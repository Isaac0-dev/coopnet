#include "client.hpp"
#include "mpacket.hpp"
#include "utils.hpp"
#include "logging.hpp"

Client* gClient = NULL;

Client::~Client() {
    if (mConnection) {
        Disconnect();
        mConnection->Disconnect();
        delete mConnection;
        mConnection = nullptr;
    }
}

bool Client::Begin(std::string aHost, uint32_t aPort)
{
    mConnection = new Connection(0);

    // setup default stun server
    mStunServer.host = "stun.l.google.com";
    mStunServer.port = 19302;

    // setup a socket
    mConnection->mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mConnection->mSocket == 0)
    {
        LOG_ERROR("Socket failed");
        return false;
    }

    // type of socket created
    mConnection->mAddress.sin_family = AF_INET;
    mConnection->mAddress.sin_addr.s_addr = GetAddrFromDomain(aHost);
    mConnection->mAddress.sin_port = htons(aPort);

    // bind the socket to localhost port 8888
    if (connect(mConnection->mSocket, (struct sockaddr*) &mConnection->mAddress, sizeof(struct sockaddr_in)) < 0) {
        LOG_ERROR("Connect failed");
        return false;
    }

    mConnection->Begin();

    return true;
}

void Client::Update() {
    if (!mConnection) { return ; }
    mConnection->Receive();

    for (auto& it : mPeers) {
        if (!it.second) { continue; }
        it.second->Update();
    }
}

void Client::Disconnect() {
    PeerEndAll();
    if (mConnection) {
        mConnection->Disconnect();
    }
}

void Client::PeerBegin(uint64_t aUserId, uint32_t aPriority) {
    mPeers[aUserId] = new Peer(this, aUserId, aPriority);
    LOG_INFO("Peer begin, count: %lu", mPeers.size());
}

void Client::PeerEnd(uint64_t aUserId) {
    Peer* peer = mPeers[aUserId];
    if (peer) {
        peer->Disconnect();
        delete peer;
    }
    mPeers.erase(aUserId);
    LOG_INFO("Peer end, count: %lu", mPeers.size());
}

void Client::PeerEndAll() {
    for (auto& it : mPeers) {
        Peer* peer = it.second;
        if (peer) {
            peer->Disconnect();
            delete peer;
        }
    }
    mPeers.clear();
    LOG_INFO("Peer end all, count: %lu", mPeers.size());
}

Peer* Client::PeerGet(uint64_t aUserId) {
    return mPeers[aUserId];
}

bool Client::PeerSend(const uint8_t* aData, size_t aDataLength) {
    bool ret = true;
    for (auto& it : mPeers) {
        Peer* peer = it.second;
        if (!peer) { continue; }
        if (!peer->Send(aData, aDataLength)) {
            ret = false;
        }
    }
    return ret && (mPeers.size() > 0);
}

bool Client::PeerSendTo(uint64_t aPeerId, const uint8_t* aData, size_t aDataLength) {
    Peer* peer = mPeers[aPeerId];
    if (!peer) { return false; }
    return peer->Send(aData, aDataLength);
}

void Client::LobbyCreate(std::string aGame, std::string aVersion, std::string aTitle, uint16_t aMaxConnections) {
    MPacketLobbyCreate(
        { .maxConnections = aMaxConnections },
        { aGame, aVersion, aTitle }
        ).Send(*mConnection);
}

void Client::LobbyJoin(uint64_t aLobbyId) {
    MPacketLobbyJoin(
        { .lobbyId = aLobbyId }
        ).Send(*mConnection);
}

void Client::LobbyLeave(uint64_t aLobbyId) {
    MPacketLobbyLeave(
        { .lobbyId = aLobbyId }
        ).Send(*mConnection);
}

void Client::LobbyListGet(std::string aGame) {
    MPacketLobbyListGet(
        {},
        { aGame }
        ).Send(*mConnection);
}