//
// Created by kingsli on 6/12/24.
//

#ifndef VANITY_PEER_EVICTION_SERVER_H
#define VANITY_PEER_EVICTION_SERVER_H

#include "client/exit_server.h"
#include "request/peer_request_server.h"
#include "peer_server.h"


namespace vanity {

/*
 * A PeerEvictionServer arbitrates
 * the eviction of misbehaving or dead peers
 */
class PeerEvictionServer:
	public virtual ExitServer,
	public virtual PeerRequestServer,
	public virtual PeerServer
{
private:
	using steady_clock = std::chrono::steady_clock;
	using time_point = std::chrono::time_point<steady_clock>;

	// represents an active issue being resolved
	struct eviction_issue_t {
		std::string issue_id;
		std::string peer_id;
		std::string reason;
		std::vector<bool> opinions;
		bool self_opinion;
		bool resolved = false;
		short expected_opinions = 0;
		time_point forget_at;
	};

	// eviction issues
	std::unordered_map<std::string, eviction_issue_t> m_eviction_issues;

	// mutex for the eviction issues map
	std::mutex m_eviction_issues_mutex;

	// all ask_evict requests that have been sent
	std::unordered_map<msg_id_t, eviction_issue_t&> m_ask_evict_requests;

	// mutex for the ask_evict requests map
	std::mutex m_ask_evict_requests_mutex;

	// insert a new eviction issue
	eviction_issue_t& insert_eviction_issue(eviction_issue_t&& issue);

	// create and add an eviction issue
	eviction_issue_t& new_eviction_issue(const std::string& peer_id, std::string reason);

	// get (or create) an eviction issue we've been asked about
	// returns the issue and whether it was created
	std::pair<eviction_issue_t&, bool> get_eviction_issue(const std::string& issue_id, const std::string& peer_id, const std::string& reason);

	// check if we have a similar issue
	bool similar_issue_exists(const std::string& peer_id, const std::string& reason);

	// get a peer given an id (inefficient, but rarely used)
	Client& peer_from_id(const std::string& id);

	// get this peer's opinion on an issue
	bool self_opinion_on(const std::string& peer_id, const std::string& reason);

	// post an ask_evict request to a peer
	msg_id_t post_ask_evict(eviction_issue_t& issue, Client &peer);

	// post an ask_evict request to all peers if that satisfy pred
	void post_ask_evict_all(eviction_issue_t& issue, const std::function<bool(Client&)>& pred);

	// remove and return an ask_evict request
	eviction_issue_t& pop_ask_evict(msg_id_t id);

	// begin the eviction process for a peer
	void begin_evict_process(Client &peer, std::string reason);

	// resolve an issue
	void resolve_issue(eviction_issue_t& issue, bool verdict);

	// try to resolve an issue
	void try_resolve_issue(eviction_issue_t& issue);

	// evict a peer for a reason
	void do_evict_peer(Client &peer, const std::string& reason);

protected:
	// check if we think a peer is dead
	virtual bool evict_opinion_dead(Client& peer) = 0;

	// get how long to wait before forgetting an issue
	virtual std::chrono::seconds forget_evict_issue_after() = 0;

	// evict a peer for misbehaving
	void evict_misbehaving_peer(Client &peer) final;

public:
	// evict a possibly dead peer
	void evict_dead_peer(Client &peer);

	// an ask_evict request was received from a peer
	void post_request_ask_evict(Context& ctx, const std::string& issue_id, const std::string& peer_id, const std::string& reason) override;

	// a reply to an ask_evict request was received from a peer
	void reply_request_ask_evict(Context& ctx, bool opinion) override;
};

} // namespace vanity

#endif //VANITY_PEER_EVICTION_SERVER_H
