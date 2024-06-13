//
// Created by kingsli on 6/12/24.
//

#include "peer_eviction_server.h"
#include "utils/random.h"


namespace vanity {

auto PeerEvictionServer::insert_eviction_issue(eviction_issue_t &&issue) -> eviction_issue_t& {
	auto issue_id = issue.issue_id;
	std::lock_guard lock{m_eviction_issues_mutex};
	return m_eviction_issues.insert({issue_id, std::move(issue)}).first->second;
}

auto PeerEvictionServer::new_eviction_issue(const std::string& peer_addr, std::string reason) -> eviction_issue_t& {
	return insert_eviction_issue({random_string(15), peer_addr, std::move(reason), {}, true});
}

auto PeerEvictionServer::get_eviction_issue(const std::string& issue_id, const std::string& peer_addr, const std::string& reason) -> std::pair<eviction_issue_t&, bool> {
	{
		std::lock_guard lock{m_eviction_issues_mutex};
		if (m_eviction_issues.contains(issue_id))
			return {m_eviction_issues.at(issue_id), false};
	}

	auto self_opinion = self_opinion_on(peer_addr, reason);
	return {insert_eviction_issue({issue_id, peer_addr, reason, {}, self_opinion}), true};
}

bool PeerEvictionServer::similar_issue_exists(const std::string &peer_addr, const std::string &reason) {
	std::lock_guard lock{m_eviction_issues_mutex};
	for (auto& [_, issue] : m_eviction_issues)
		if (issue.peer_addr == peer_addr && issue.reason == reason)
			return true;

	return false;
}

Client& PeerEvictionServer::peer_from_addr(const std::string &addr) {
	for (auto p : get_peers())
		if (session_addr(*p) == addr)
			return *p;

	throw std::runtime_error("no peer with address " + addr + " found");
}

bool PeerEvictionServer::self_opinion_on(const std::string &peer_addr, const std::string &reason) {
	auto& peer = peer_from_addr(peer_addr);
	if (reason == "misbehaving")
		return evict_opinion_misbehaving(peer);
	else if (reason == "dead")
		return evict_opinion_dead(peer);
	else
		throw std::runtime_error("unknown evict reason " + reason);
}

msg_id_t PeerEvictionServer::post_ask_evict(eviction_issue_t &issue, Client &peer) {
	return post(peer, peer_op_t::ASK_EVICT, issue.issue_id, issue.peer_addr, issue.reason);
}

void PeerEvictionServer::post_ask_evict_all(eviction_issue_t &issue, const std::function<bool(Client&)>& pred) {
	auto peers = get_peers();
	issue.opinions.reserve(peers.size() + 1);
	issue.opinions.push_back(issue.self_opinion);
	issue.expected_opinions = peers.size() / 2 + 1;

	std::lock_guard lock{m_ask_evict_requests_mutex};
	for (auto p : peers)
		if (pred(*p)) {
			auto id = post_ask_evict(issue, *p);
			m_ask_evict_requests.insert({id, issue});
		}
}

auto PeerEvictionServer::pop_ask_evict(msg_id_t id) -> eviction_issue_t& {
	std::lock_guard lock{m_ask_evict_requests_mutex};
	auto& issue = m_ask_evict_requests.at(id);
	m_ask_evict_requests.erase(id);
	return issue;
}

void PeerEvictionServer::begin_evict_process(Client &peer, std::string reason) {
	auto& peer_addr = session_addr(peer);
	if (similar_issue_exists(peer_addr, reason))
		return;

	auto &issue = new_eviction_issue(peer_addr, std::move(reason));
	post_ask_evict_all(issue, [&peer](Client &p) { return &p != &peer; });
}

void PeerEvictionServer::resolve_issue(eviction_issue_t &issue, bool verdict) {
	issue.forget_at = steady_clock::now() + forget_evict_issue_after();
	issue.resolved = true;

	if (verdict)
		do_evict_peer(peer_from_addr(issue.peer_addr), issue.reason);
}

void PeerEvictionServer::try_resolve_issue(eviction_issue_t &issue) {
	if (issue.resolved)
		return;

	auto true_count = std::count(issue.opinions.begin(), issue.opinions.end(), true);
	auto false_count = issue.opinions.size() - true_count;

	if (true_count >= issue.expected_opinions)
		resolve_issue(issue, true);
	else if (false_count >= issue.expected_opinions)
		resolve_issue(issue, false);
}

void PeerEvictionServer::do_evict_peer(Client &peer, const std::string &reason) {
	logger().info("evicting peer " + session_addr(peer) + " for " + reason);
	auto& tcp_peer = to_tcp(peer);
	forget_peer(tcp_peer);
	tcp_peer.close();
}

void PeerEvictionServer::evict_misbehaving_peer(Client &peer) {
	begin_evict_process(peer, "misbehaving");
}

void PeerEvictionServer::evict_dead_peer(Client &peer) {
	begin_evict_process(peer, "dead");
}

void PeerEvictionServer::post_request_ask_evict(Context& ctx, const std::string& issue_id, const std::string& peer_addr, const std::string& reason) {
	auto [issue, created] = get_eviction_issue(issue_id, peer_addr, reason);
	reply(ctx, issue.self_opinion);

	if (created)
		post_ask_evict_all(issue, [&peer_addr](Client &p) { return session_addr(p) != peer_addr; });
}

void PeerEvictionServer::reply_request_ask_evict(Context& ctx, bool opinion) {
	auto& issue = pop_ask_evict(ctx.msg_id);
	issue.opinions.push_back(opinion);
	try_resolve_issue(issue);
}

} // namespace vanity
