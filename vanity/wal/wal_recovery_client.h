//
// Created by kingsli on 2/12/24.
//

#ifndef VANITY_WAL_RECOVERY_CLIENT_H
#define VANITY_WAL_RECOVERY_CLIENT_H

#include "client/client.h"

namespace vanity::wal {

class WalRecoveryClient : public Client
{
private:
	// the client's session info
	struct session_info m_session_info;

public:
	// check if the client has permission to perform an op
	// this always returns true
	bool has_perm(operation_t op) const override;

	// return the client's session info
	struct session_info& session_info() override;

	// write a sendable to the client
	// this does exactly nothing
	void write(WriteManager& manager, Sendable&& sendable) override;

};

} // namespace vanity::wal

#endif //VANITY_WAL_RECOVERY_CLIENT_H
