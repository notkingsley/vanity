//
// Created by kingsli on 12/31/23.
//

#ifndef VANITY_PUBSUB_SERVER_H
#define VANITY_PUBSUB_SERVER_H

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "client/client_server.h"
#include "event_server.h"
#include "request/request_handler.h"
#include "response/response_server.h"
#include "session_server.h"


namespace vanity {

/*
 * A pubsub server handles the vanity pubsub protocol.
 */
class PubSubServer:
	public virtual ClientServer,
	public virtual EventServer,
	public virtual RequestHandler,
	public virtual ResponseServer,
	public virtual SessionServer
{
public:
	// data about to be published
	struct PublishData {
		std::string channel;
		std::string message;
	};

private:
	// a map of channels to clients subscribed to that channel
	std::unordered_map<std::string, std::unordered_set<Client*>> m_subscriptions;

	// mutex for m_subscriptions
	std::mutex m_subscriptions_mutex;

	// a queue of messages to be published
	std::vector<PublishData> m_publish_queue;

	// mutex for m_publish_queue
	std::mutex m_publish_queue_mutex;

public:
	// a subscribe request was received from a client
	void request_subscribe(Client& client, const std::string& channel) override;

	// a subscribed request was received from a client
	void request_subscribed(Client& client) override;

	// an unsubscribe request was received from a client
	void request_unsubscribe(Client& client, const std::string& channel) override;

	// an unsubscribe_all request was received from a client
	void request_unsubscribe_all(Client& client) override;

	// a publish request was received from a client
	void request_publish(Client& client, const std::string& channel, const std::string& message) override;

	// a publish event was received
	void event_publish() override;

private:
	// erase a client from a channel
	// assumes m_subscriptions_mutex is locked
	void erase_subscription(Client& client, const std::string& channel);

	// insert a client into a channel
	// assumes m_subscriptions_mutex is locked
	void insert_subscription(Client& client, const std::string& channel);

	// publish a message to a channel
	// assumes m_subscriptions_mutex is locked
	void publish(const PublishData& data);

	// the pubsub hook pre-deleting a client
	// this removes the tcp_client from all channels
	void pre_client_delete_pubsub(TcpClient& client) override;
};

} // namespace vanity

#endif //VANITY_PUBSUB_SERVER_H
