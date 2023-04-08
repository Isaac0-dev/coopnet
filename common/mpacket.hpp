#pragma once

#include <cstdint>
#include <vector>

#define MPACKET_PROTOCOL_VERSION 1
#define MPACKET_MAX_SIZE ((size_t)5100)

// forward declarations
class Connection;
class Lobby;

enum MPacketType {
    MPACKET_NONE,
    MPACKET_JOINED,
    MPACKET_LOBBY_CREATE,
    MPACKET_LOBBY_CREATED,
    MPACKET_LOBBY_JOIN,
    MPACKET_LOBBY_JOINED,
    MPACKET_LOBBY_LEAVE,
    MPACKET_LOBBY_LEFT,
    MPACKET_LOBBY_LIST_GET,
    MPACKET_LOBBY_LIST_GOT,
    MPACKET_PEER_SDP,
    MPACKET_PEER_CANDIDATE,
    MPACKET_STUN_TURN,
    MPACKET_ERROR,
    MPACKET_MAX,
};

enum MPacketSendType {
    MSEND_TYPE_CLIENT,
    MSEND_TYPE_SERVER,
    MSEND_TYPE_BOTH,
};

#pragma pack(1)

typedef struct {
    uint16_t packetType;
    uint16_t dataSize;
    uint16_t stringSize;
} MPacketHeader;

typedef struct {
    uint64_t userId;
    uint32_t version;
} MPacketJoinedData;

typedef struct {
    uint16_t maxConnections;
} MPacketLobbyCreateData;

typedef struct {
    uint64_t lobbyId;
} MPacketLobbyCreatedData;

typedef struct {
    uint64_t lobbyId;
} MPacketLobbyJoinData;

typedef struct {
    uint64_t lobbyId;
    uint64_t userId;
} MPacketLobbyJoinedData;

typedef struct {
    uint64_t lobbyId;
} MPacketLobbyLeaveData;

typedef struct {
    uint64_t lobbyId;
    uint64_t userId;
} MPacketLobbyLeftData;

typedef struct {
} MPacketLobbyListGetData;

typedef struct {
    uint64_t lobbyId;
    uint64_t ownerId;
    uint16_t connections;
    uint16_t maxConnections;
} MPacketLobbyListGotData;

typedef struct {
    uint64_t lobbyId;
    uint64_t userId;
} MPacketPeerSdpData;

typedef struct {
    uint64_t lobbyId;
    uint64_t userId;
} MPacketPeerCandidateData;

typedef struct {
    uint8_t isStun;
    uint16_t port;
} MPacketStunTurnData;

typedef struct {
    uint16_t errorNumber;
} MPacketErrorData;

#pragma pack()

typedef struct {
    enum MPacketType packetType;
    uint16_t stringCount;
    enum MPacketSendType sendType;
} MPacketImplSettings;

class MPacket {
    private:
    protected:
        void* mVoidData = NULL;
        uint16_t mVoidDataSize = 0;
        std::vector<std::string> mStringData;

    public:
        void Send(Connection& connection);
        void Send(Lobby& lobby);
        static void Read(Connection* connection, uint8_t* aData, uint16_t* aDataSize, uint16_t aMaxDataSize);
        static void Process(Connection* connection, uint8_t* aData);
        virtual bool Receive(Connection* connection) { return false; };
        virtual MPacketImplSettings GetImplSettings() { return {
            .packetType = MPACKET_NONE,
            .stringCount = 0,
            .sendType = MSEND_TYPE_CLIENT
        };}

};

template<typename T>
class MPacketImpl : public MPacket {
    protected:
        T mData;
    public:
        MPacketImpl() {
            mVoidData = &mData;
            mVoidDataSize = sizeof(T);
        }

        MPacketImpl(T aData) {
            mData = aData;
            mVoidData = &mData;
            mVoidDataSize = sizeof(T);
        }

        MPacketImpl(T aData, std::vector<std::string> aStringData) {
            mData = aData;
            mVoidData = &mData;
            mVoidDataSize = sizeof(T);
            mStringData = aStringData;
        }

};

class MPacketJoined : public MPacketImpl<MPacketJoinedData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_JOINED,
            .stringCount = 0,
            .sendType = MSEND_TYPE_SERVER
        };}
        bool Receive(Connection* connection) override;
};

class MPacketLobbyCreate : public MPacketImpl<MPacketLobbyCreateData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_LOBBY_CREATE,
            .stringCount = 3,
            .sendType = MSEND_TYPE_CLIENT
        };}
        bool Receive(Connection* connection) override;
};

class MPacketLobbyCreated : public MPacketImpl<MPacketLobbyCreatedData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_LOBBY_CREATED,
            .stringCount = 3,
            .sendType = MSEND_TYPE_SERVER
        };}
        bool Receive(Connection* connection) override;
};

class MPacketLobbyJoin : public MPacketImpl<MPacketLobbyJoinData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_LOBBY_JOIN,
            .stringCount = 0,
            .sendType = MSEND_TYPE_CLIENT
        };}
        bool Receive(Connection* connection) override;
};

class MPacketLobbyJoined : public MPacketImpl<MPacketLobbyJoinedData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_LOBBY_JOINED,
            .stringCount = 0,
            .sendType = MSEND_TYPE_SERVER
        };}
        bool Receive(Connection* connection) override;
};

class MPacketLobbyLeave : public MPacketImpl<MPacketLobbyLeaveData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_LOBBY_LEAVE,
            .stringCount = 0,
            .sendType = MSEND_TYPE_CLIENT
        };}
        bool Receive(Connection* connection) override;
};

class MPacketLobbyLeft : public MPacketImpl<MPacketLobbyLeftData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_LOBBY_LEFT,
            .stringCount = 0,
            .sendType = MSEND_TYPE_SERVER
        };}
        bool Receive(Connection* connection) override;
};

class MPacketLobbyListGet : public MPacketImpl<MPacketLobbyListGetData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_LOBBY_LIST_GET,
            .stringCount = 1,
            .sendType = MSEND_TYPE_CLIENT
        };}
        bool Receive(Connection* connection) override;
};

class MPacketLobbyListGot : public MPacketImpl<MPacketLobbyListGotData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_LOBBY_LIST_GOT,
            .stringCount = 3,
            .sendType = MSEND_TYPE_SERVER
        };}
        bool Receive(Connection* connection) override;
};

class MPacketPeerSdp : public MPacketImpl<MPacketPeerSdpData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_PEER_SDP,
            .stringCount = 1,
            .sendType = MSEND_TYPE_BOTH
        };}
        bool Receive(Connection* connection) override;
};

class MPacketPeerCandidate : public MPacketImpl<MPacketPeerCandidateData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_PEER_CANDIDATE,
            .stringCount = 1,
            .sendType = MSEND_TYPE_BOTH
        };}
        bool Receive(Connection* connection) override;
};

class MPacketStunTurn : public MPacketImpl<MPacketStunTurnData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_STUN_TURN,
            .stringCount = 3,
            .sendType = MSEND_TYPE_SERVER
        };}
        bool Receive(Connection* connection) override;
};

class MPacketError : public MPacketImpl<MPacketErrorData> {
    public:
        using MPacketImpl::MPacketImpl;
        MPacketImplSettings GetImplSettings() override { return {
            .packetType = MPACKET_ERROR,
            .stringCount = 0,
            .sendType = MSEND_TYPE_SERVER
        };}
        bool Receive(Connection* connection) override;
};

