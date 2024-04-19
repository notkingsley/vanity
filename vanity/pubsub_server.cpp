//
// Created by kingsli on 12/31/23.
//

#include "pubsub_server.h"

namespace vanity {

void PubSubServer::request_subscribe(Client &client, const std::string &channel) {
	auto &channels = session_channels(client);
	{
		std::lock_guard lock(m_subscriptions_mutex);
		insert_subscription(client, channel);
	}
	channels.insert(channel);
	send(client, ok(channels.size()));
}

void PubSubServer::request_subscribed(Client &client) {
	auto &channels_set = session_channels(client);
	std::vector<std::string> channel_arr{channels_set.begin(), channels_set.end()};
	send(client, ok(channel_arr));
}

void PubSubServer::request_unsubscribe(Client &client, const std::string &channel) {
	auto &channels = session_channels(client);
	{
		std::lock_guard lock(m_subscriptions_mutex);
		erase_subscription(client, channel);
	}
	channels.erase(channel);
	send(client, ok(channels.size()));
}

void PubSubServer::request_unsubscribe_all(Client &client) {
	auto &channels = session_channels(client);
	{
		std::lock_guard lock(m_subscriptions_mutex);
		for (auto& channel : channels)
			erase_subscription(client, channel);
	}
	channels.clear();
	send(client, ok());
}

void PubSubServer::request_publish(Client &client, const std::string &channel, const std::string &message) {
	{
		std::lock_guard lock(m_publish_queue_mutex);
		m_publish_queue.emplace_back(channel, message);
	}
	push_event(server_event::publish);
	send(client, ok());
}

void PubSubServer::event_publish() {
	std::vector<PublishData> publish_queue;
	{
		std::lock_guard lock(m_publish_queue_mutex);
		if (m_publish_queue.empty())
			return;
		std::swap(publish_queue, m_publish_queue);
	}

	std::lock_guard lock(m_subscriptions_mutex);
	for (auto& data : publish_queue)
		publish(data);
}

void PubSubServer::erase_subscription(Client &client, const std::string &channel) {
	if (not m_subscriptions.contains(channel))
		return;

	m_subscriptions[channel].erase(&client);
	if (m_subscriptions[channel].empty())
		m_subscriptions.erase(channel);
}

void PubSubServer::insert_subscription(Client &client, const std::string &channel) {
	m_subscriptions[channel].insert(&client);
}

template<>
struct type_to_string<PubSubServer::PublishData> {
	static constexpr const char* value = ":PUBLISH ";
};

template<>
Response& Response::serialize(const PubSubServer::PublishData& data) {
	return serialize_type<PubSubServer::PublishData>()
		.serialize_string_body(data.channel)
		.serialize_string_body(data.message);
}

void PubSubServer::publish(const PublishData &data) {
	if (not m_subscriptions.contains(data.channel))
		return;

	auto response = async(data);
	for (auto& client : m_subscriptions[data.channel])
		send(*client, Response(response)); // copy response
}

void PubSubServer::pre_client_delete_pubsub(TcpClient &client) {
	std::lock_guard lock {m_subscriptions_mutex};
	for (auto& channel : session_channels(client))
		erase_subscription(client, channel);
}

} // namespace vanity
