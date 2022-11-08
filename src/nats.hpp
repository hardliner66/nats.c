// This file has been auto-generated.
// DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING

// Copyright 2015-2020 The NATS Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NATS_HPP_
#define NATS_HPP_

#include "nats.h"
#include <exception>

namespace nats {
class Connection;
class Statistics;
class Subscription;
class Msg;
class Options;
class Inbox;

/** \brief Callback used to deliver messages to the application.
 *
 * This is the callback that one provides when creating an asynchronous
 * subscription. The library will invoke this callback for each message
 * arriving through the subscription's connection.
 *
 * \warning If this callback is setup for a subject that is used as the reply
 * subject to #natsConnection_PublishRequest calls (and its variants), it
 * is possible to get an empty message with a header "Status" with value
 * "503" that is sent by the server when there were no responders on the
 * request's subject. Use #natsMsg_IsNoResponders to know if that is the case.
 *
 * @see natsConnection_Subscribe()
 * @see natsConnection_QueueSubscribe()
 * @see natsMsg_IsNoResponders()
 */
template<typename T>
using MsgHandler = void (T::*)(Connection &, Subscription &, Msg &&);

template<typename T, MsgHandler<T> callback> void
MsgHandlerCallback(natsConnection * nc, natsSubscription * sub, natsMsg * msg, void * closure);

/** \brief Callback used to notify the user of asynchronous connection events.
 *
 * This callback is used for asynchronous events such as disconnected
 * and closed connections.
 *
 * @see natsOptions_SetClosedCB()
 * @see natsOptions_SetDisconnectedCB()
 * @see natsOptions_SetReconnectedCB()
 *
 * \warning Such callback is invoked from a dedicated thread and the state
 *          of the connection that triggered the event may have changed since
 *          that event was generated.
 */
template<typename T>
using ConnectionHandler = void (T::*)(Connection &);

template<typename T, ConnectionHandler<T> callback> void
ConnectionHandlerCallback(natsConnection * nc, void * closure);

/** \brief Callback used to notify the user of errors encountered while processing
 *         inbound messages.
 *
 * This callback is used to process asynchronous errors encountered while processing
 * inbound messages, such as #NATS_SLOW_CONSUMER.
 */
template<typename T>
using ErrHandler = void (T::*)(Connection &, Subscription &, natsStatus err);

template<typename T, ErrHandler<T> callback> void
ErrHandlerCallback(natsConnection * nc, natsSubscription * subscription, natsStatus err, void * closure);

/** \brief Callback used to fetch and return account signed user JWT.
 *
 * This handler is invoked when connecting and reconnecting. It should
 * return the user JWT that will be sent to the server.
 *
 * The user JWT is returned as a string that is allocated by the user and is
 * freed by the library after the handler is invoked.
 *
 * If the user is unable to return the JWT, a status other than `NATS_OK` should
 * be returned (we recommend `NATS_ERR`). A custom error message can be returned
 * through `customErrTxt`. The user must allocate the memory for this error
 * message and the library will free it after the invocation of the handler.
 *
 * \warning There may be repeated invocations of this handler for a given connection
 * so it is crucial to always return a copy of the user JWT maintained by the
 * application, since again, the library will free the memory pointed by `userJWT`
 * after each invocation of this handler.
 *
 * @see natsOptions_SetUserCredentialsCallbacks()
 * @see natsOptions_SetUserCredentialsFromFiles()
 */
template<typename T>
using UserJWTHandler = natsStatus (T::*)(char ** userJWT, char ** customErrTxt);

template<typename T, UserJWTHandler<T> callback> natsStatus
UserJWTHandlerCallback(char ** userJWT, char ** customErrTxt, void * closure);

/** \brief Callback used to sign a nonce sent by the server.
 *
 * This handler is invoked when connecting and reconnecting. It should
 * sign the given `nonce` and return a raw signature through `signature` and
 * specify how many characters the signature has using `signatureLength`.
 *
 * The memory pointed by `signature` must be allocated by the user and
 * will be freed by the library after each invocation of this handler.
 *
 * If the user is unable to sign, a status other than `NATS_OK` (we recommend
 * `NATS_ERR`) should be returned. A custom error message can be returned
 * through `customErrTxt`. The user must allocate the memory for this error
 * message and the library will free it after the invocation of this handler.
 *
 * The library will base64 encode this raw signature and send that to the server.
 *
 * \warning There may be repeated invocations of this handler for a given connection
 * so it is crucial to always return a copy of the signature, since again,
 * the library will free the memory pointed by `signature` after each invocation
 * of this handler.
 *
 * @see natsOptions_SetUserCredentialsCallbacks()
 * @see natsOptions_SetUserCredentialsFromFiles()
 * @see natsOptions_SetNKey()
 */
template<typename T>
using SignatureHandler = natsStatus (T::*)(char ** customErrTxt, unsigned char ** signature, int * signatureLength, const char * nonce);

template<typename T, SignatureHandler<T> callback> natsStatus
SignatureHandlerCallback(char ** customErrTxt, unsigned char ** signature, int * signatureLength, const char * nonce, void * closure);

/** \brief Callback used to build a token on connections and reconnections.
 *
 * This is the function that one provides to build a different token at each reconnect.
 *
 * @see natsOptions_SetTokenHandler()
 *
 * \warning Such callback is invoked synchronously from the connection thread.
 */
template<typename T>
using TokenHandler = void (T::*)();

template<typename T, TokenHandler<T> callback> void
TokenHandlerCallback(void * closure);

/** \brief Callback used to notify that an object lifecycle is complete.
 *
 * Currently used for asynchronous #natsSubscription objects. When set, this callback will
 * be invoked after the subscription is closed and the message handler has returned.
 *
 * @see natsSubscription_SetOnCompleteCB()
 */
template<typename T>
using OnCompleteCB = void (T::*)();

template<typename T, OnCompleteCB<T> callback> void
OnCompleteCBCallback(void * closure);

/** \brief Callback used to specify how long to wait between reconnects.
 *
 * This callback is used to get from the user the desired delay the library
 * should pause before attempting to reconnect again. Note that this is invoked
 * after the library tried the whole list of URLs and failed to reconnect.
 *
 * \note This callback is invoked from the connection reconnect thread and waits
 * for user input. It should not block and instead quickly return the desired
 * reconnect delay.
 * The state of the connection is disconnected when this callback is invoked.
 * Not much can be done with the passed connection, but user can call
 * #natsConnection_Close() if desired. This will abort the reconnect attempts
 * and close the connection.
 *
 * @param nc the pointer to the #natsConnection invoking this handler.
 * @param attempts the number of times the library tried the whole list of server URLs.
 * @param closure an optional pointer to a user defined object that was specified when
 * registering the callback.
 * @return the number of milliseconds to wait before trying to reconnect.
 */
template<typename T>
using CustomReconnectDelayHandler = int64_t (T::*)(Connection &, int attempts);

template<typename T, CustomReconnectDelayHandler<T> callback> int64_t
CustomReconnectDelayHandlerCallback(natsConnection * nc, int attempts, void * closure);

class Exception : public std::exception {
    natsStatus status;

public:
    Exception(natsStatus s) : status(s)
    {
    }

    const natsStatus&
    code()
    {
      return status;
    }

    const char *
    what() const noexcept
    {
      return natsStatus_GetText(status);
    }

    static void
    CheckResult(natsStatus status)
    {
        if (status != NATS_OK)
            throw Exception(status);
    }
};

/** \brief A connection to a `NATS Server`.
 *
 * A #natsConnection represents a bare connection to a `NATS Server`. It will
 * send and receive byte array payloads.
 */
class Connection {
    class WithoutDestuction;
    friend class Statistics;
    friend class Subscription;
    friend class Msg;
    friend class Options;
    friend class Inbox;
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(natsConnection * nc, natsSubscription * sub, natsMsg * msg, void * closure);
    template<typename T, ConnectionHandler<T> callback> friend void
    ConnectionHandlerCallback(natsConnection * nc, void * closure);
    template<typename T, ErrHandler<T> callback> friend void
    ErrHandlerCallback(natsConnection * nc, natsSubscription * subscription, natsStatus err, void * closure);
    template<typename T, UserJWTHandler<T> callback> friend natsStatus
    UserJWTHandlerCallback(char ** userJWT, char ** customErrTxt, void * closure);
    template<typename T, SignatureHandler<T> callback> friend natsStatus
    SignatureHandlerCallback(char ** customErrTxt, unsigned char ** signature, int * signatureLength, const char * nonce, void * closure);
    template<typename T, TokenHandler<T> callback> friend void
    TokenHandlerCallback(void * closure);
    template<typename T, OnCompleteCB<T> callback> friend void
    OnCompleteCBCallback(void * closure);
    template<typename T, CustomReconnectDelayHandler<T> callback> friend int64_t
    CustomReconnectDelayHandlerCallback(natsConnection * nc, int attempts, void * closure);
    natsConnection * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit Connection(natsConnection* ptr) : self(ptr)
    {
    }

    Connection(Connection&& other) : self(other.Release())
    {
    }

    /** \brief Connects to a `NATS Server` using the provided options.
    *
    * @see #natsConnection_Connect()
    */
    Connection(natsOptions * options);

    /** \brief Process a read event when using external event loop.
    *
    * @see #natsConnection_ProcessReadEvent()
    */
    void
    ProcessReadEvent();

    /** \brief Process a write event when using external event loop.
    *
    * @see #natsConnection_ProcessWriteEvent()
    */
    void
    ProcessWriteEvent();

    /** \brief Connects to a `NATS Server` using any of the URL from the given list.
    *
    * @see #natsConnection_ConnectTo()
    */
    Connection(const char * urls);

    /** \brief Test if connection has been closed.
    *
    * @see #natsConnection_IsClosed()
    */
    bool
    IsClosed();

    /** \brief Test if connection is reconnecting.
    *
    * @see #natsConnection_IsReconnecting()
    */
    bool
    IsReconnecting();

    /** \brief Test if connection is draining.
    *
    * @see #natsConnection_IsDraining()
    */
    bool
    IsDraining();

    /** \brief Returns the current state of the connection.
    *
    * @see #natsConnection_Status()
    */
    natsConnStatus
    Status();

    /** \brief Returns the number of bytes to be sent to the server.
    *
    * @see #natsConnection_Buffered()
    */
    int
    Buffered();

    /** \brief Flushes the connection.
    *
    * @see #natsConnection_Flush()
    */
    void
    Flush();

    /** \brief Flushes the connection with a given timeout.
    *
    * @see #natsConnection_FlushTimeout()
    */
    void
    FlushTimeout(int64_t timeout);

    /** \brief Returns the maximum message payload.
    *
    * @see #natsConnection_GetMaxPayload()
    */
    int64_t
    GetMaxPayload();

    /** \brief Gets the connection statistics.
    *
    * @see #natsConnection_GetStats()
    */
    void
    GetStats(natsStatistics * stats);

    /** \brief Gets the URL of the currently connected server.
    *
    * @see #natsConnection_GetConnectedUrl()
    */
    void
    GetConnectedUrl(char * buffer, size_t bufferSize);

    /** \brief Gets the server Id.
    *
    * @see #natsConnection_GetConnectedServerId()
    */
    void
    GetConnectedServerId(char * buffer, size_t bufferSize);

    /** \brief Returns the list of server URLs known to this connection.
    *
    * @see #natsConnection_GetServers()
    */
    void
    GetServers(char *** servers, int * count);

    /** \brief Returns the list of discovered server URLs.
    *
    * @see #natsConnection_GetDiscoveredServers()
    */
    void
    GetDiscoveredServers(char *** servers, int * count);

    /** \brief Gets the last connection error.
    *
    * @see #natsConnection_GetLastError()
    */
    void
    GetLastError(const char ** lastError);

    /** \brief Gets the current client ID assigned by the server.
    *
    * @see #natsConnection_GetClientID()
    */
    void
    GetClientID(uint64_t * cid);

    /** \brief Drains the connection with default timeout.
    *
    * @see #natsConnection_Drain()
    */
    void
    Drain();

    /** \brief Drains the connection with given timeout.
    *
    * @see #natsConnection_DrainTimeout()
    */
    void
    DrainTimeout(int64_t timeout);

    /** \brief Signs any 'message' using the connection's user credentials.
    *
    * @see #natsConnection_Sign()
    */
    void
    Sign(const unsigned char * message, int messageLen, unsigned char sig[64]);

    /** \brief Returns the client's IP address as reported by the server.
    *
    * @see #natsConnection_GetClientIP()
    */
    void
    GetClientIP(char ** ip);

    /** \brief Returns the round trip time between this client and the server.
    *
    * @see #natsConnection_GetRTT()
    */
    void
    GetRTT(int64_t * rtt);

    /** \brief Returns if the connection to current server supports headers.
    *
    * @see #natsConnection_HasHeaderSupport()
    */
    void
    HasHeaderSupport();

    /** \brief Returns the connection local IP and port.
    *
    * @see #natsConnection_GetLocalIPAndPort()
    */
    void
    GetLocalIPAndPort(char ** ip, int * port);

    /** \brief Closes the connection.
    *
    * @see #natsConnection_Close()
    */
    void
    Close();

    /** \brief Destroys the connection object.
    *
    * @see #natsConnection_Destroy()
    */
    ~Connection();

    /** \brief Publishes data on a subject.
    *
    * @see #natsConnection_Publish()
    */
    void
    Publish(const char * subj, const void * data, int dataLen);

    /** \brief Publishes a string on a subject.
    *
    * @see #natsConnection_PublishString()
    */
    void
    PublishString(const char * subj, const char * str);

    /** \brief Publishes a message on a subject.
    *
    * @see #natsConnection_PublishMsg()
    */
    void
    PublishMsg(natsMsg * msg);

    /** \brief Publishes data on a subject expecting replies on the given reply.
    *
    * @see #natsConnection_PublishRequest()
    */
    void
    PublishRequest(const char * subj, const char * reply, const void * data, int dataLen);

    /** \brief Publishes a string on a subject expecting replies on the given reply.
    *
    * @see #natsConnection_PublishRequestString()
    */
    void
    PublishRequestString(const char * subj, const char * reply, const char * str);

    /** \brief Sends a request and waits for a reply.
    *
    * @see #natsConnection_Request()
    */
    Msg
    Request(const char * subj, const void * data, int dataLen, int64_t timeout);

    /** \brief Sends a request (as a string) and waits for a reply.
    *
    * @see #natsConnection_RequestString()
    */
    Msg
    RequestString(const char * subj, const char * str, int64_t timeout);

    /** \brief Sends a request based on the given `requestMsg` and waits for a reply.
    *
    * @see #natsConnection_RequestMsg()
    */
    Msg
    RequestMsg(natsMsg * requestMsg, int64_t timeout);

    /** \brief Creates an asynchronous subscription.
    *
    * @see #natsConnection_Subscribe()
    */
    Subscription
    Subscribe(const char * subject, natsMsgHandler cb, void * cbClosure);

    template<typename T1, MsgHandler<T1> callback1> Subscription
    Subscribe(const char * subject, T1 * cbClosure);

    /** \brief Creates an asynchronous subscription with a timeout.
    *
    * @see #natsConnection_SubscribeTimeout()
    */
    Subscription
    SubscribeTimeout(const char * subject, int64_t timeout, natsMsgHandler cb, void * cbClosure);

    template<typename T1, MsgHandler<T1> callback1> Subscription
    SubscribeTimeout(const char * subject, int64_t timeout, T1 * cbClosure);

    /** \brief Creates a synchronous subcription.
    *
    * @see #natsConnection_SubscribeSync()
    */
    Subscription
    SubscribeSync(const char * subject);

    /** \brief Creates an asynchronous queue subscriber.
    *
    * @see #natsConnection_QueueSubscribe()
    */
    Subscription
    QueueSubscribe(const char * subject, const char * queueGroup, natsMsgHandler cb, void * cbClosure);

    template<typename T1, MsgHandler<T1> callback1> Subscription
    QueueSubscribe(const char * subject, const char * queueGroup, T1 * cbClosure);

    /** \brief Creates an asynchronous queue subscriber with a timeout.
    *
    * @see #natsConnection_QueueSubscribeTimeout()
    */
    Subscription
    QueueSubscribeTimeout(const char * subject, const char * queueGroup, int64_t timeout, natsMsgHandler cb, void * cbClosure);

    template<typename T1, MsgHandler<T1> callback1> Subscription
    QueueSubscribeTimeout(const char * subject, const char * queueGroup, int64_t timeout, T1 * cbClosure);

    /** \brief Creates a synchronous queue subscriber.
    *
    * @see #natsConnection_QueueSubscribeSync()
    */
    Subscription
    QueueSubscribeSync(const char * subject, const char * queueGroup);

    /** \brief Returns a new JetStream context.
    *
    * @see #natsConnection_JetStream()
    */
    void
    JetStream(jsCtx ** js, jsOptions * opts);

    operator const natsConnection * () const
    {
        return self;
    }

    operator natsConnection * ()
    {
        return self;
    }

    [[nodiscard]] natsConnection *
    Release()
    {
        natsConnection * ret = self;
        self = nullptr;
        return ret;
    }
};

class Connection::WithoutDestuction : public Connection {
public:
    WithoutDestuction(natsConnection * ptr) : Connection(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

/** \brief Statistics of a #natsConnection
 *
 * Tracks various statistics received and sent on a connection,
 * including counts for messages and bytes.
 */
class Statistics {
    class WithoutDestuction;
    friend class Connection;
    friend class Subscription;
    friend class Msg;
    friend class Options;
    friend class Inbox;
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(natsConnection * nc, natsSubscription * sub, natsMsg * msg, void * closure);
    template<typename T, ConnectionHandler<T> callback> friend void
    ConnectionHandlerCallback(natsConnection * nc, void * closure);
    template<typename T, ErrHandler<T> callback> friend void
    ErrHandlerCallback(natsConnection * nc, natsSubscription * subscription, natsStatus err, void * closure);
    template<typename T, UserJWTHandler<T> callback> friend natsStatus
    UserJWTHandlerCallback(char ** userJWT, char ** customErrTxt, void * closure);
    template<typename T, SignatureHandler<T> callback> friend natsStatus
    SignatureHandlerCallback(char ** customErrTxt, unsigned char ** signature, int * signatureLength, const char * nonce, void * closure);
    template<typename T, TokenHandler<T> callback> friend void
    TokenHandlerCallback(void * closure);
    template<typename T, OnCompleteCB<T> callback> friend void
    OnCompleteCBCallback(void * closure);
    template<typename T, CustomReconnectDelayHandler<T> callback> friend int64_t
    CustomReconnectDelayHandlerCallback(natsConnection * nc, int attempts, void * closure);
    natsStatistics * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit Statistics(natsStatistics* ptr) : self(ptr)
    {
    }

    Statistics(Statistics&& other) : self(other.Release())
    {
    }

    /** \brief Creates a #natsStatistics object.
    *
    * @see #natsStatistics_Create()
    */
    Statistics();

    /** \brief Extracts the various statistics values.
    *
    * @see #natsStatistics_GetCounts()
    */
    void
    GetCounts(uint64_t * inMsgs, uint64_t * inBytes, uint64_t * outMsgs, uint64_t * outBytes, uint64_t * reconnects) const;

    /** \brief Destroys the #natsStatistics object.
    *
    * @see #natsStatistics_Destroy()
    */
    ~Statistics();

    operator const natsStatistics * () const
    {
        return self;
    }

    operator natsStatistics * ()
    {
        return self;
    }

    [[nodiscard]] natsStatistics *
    Release()
    {
        natsStatistics * ret = self;
        self = nullptr;
        return ret;
    }
};

class Statistics::WithoutDestuction : public Statistics {
public:
    WithoutDestuction(natsStatistics * ptr) : Statistics(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

/** \brief Interest on a given subject.
 *
 * A #natsSubscription represents interest in a given subject.
 */
class Subscription {
    class WithoutDestuction;
    friend class Connection;
    friend class Statistics;
    friend class Msg;
    friend class Options;
    friend class Inbox;
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(natsConnection * nc, natsSubscription * sub, natsMsg * msg, void * closure);
    template<typename T, ConnectionHandler<T> callback> friend void
    ConnectionHandlerCallback(natsConnection * nc, void * closure);
    template<typename T, ErrHandler<T> callback> friend void
    ErrHandlerCallback(natsConnection * nc, natsSubscription * subscription, natsStatus err, void * closure);
    template<typename T, UserJWTHandler<T> callback> friend natsStatus
    UserJWTHandlerCallback(char ** userJWT, char ** customErrTxt, void * closure);
    template<typename T, SignatureHandler<T> callback> friend natsStatus
    SignatureHandlerCallback(char ** customErrTxt, unsigned char ** signature, int * signatureLength, const char * nonce, void * closure);
    template<typename T, TokenHandler<T> callback> friend void
    TokenHandlerCallback(void * closure);
    template<typename T, OnCompleteCB<T> callback> friend void
    OnCompleteCBCallback(void * closure);
    template<typename T, CustomReconnectDelayHandler<T> callback> friend int64_t
    CustomReconnectDelayHandlerCallback(natsConnection * nc, int attempts, void * closure);
    natsSubscription * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit Subscription(natsSubscription* ptr) : self(ptr)
    {
    }

    Subscription(Subscription&& other) : self(other.Release())
    {
    }

    /** \brief Enables the No Delivery Delay mode.
    *
    * @see #natsSubscription_NoDeliveryDelay()
    */
    void
    NoDeliveryDelay();

    /** \brief Returns the next available message.
    *
    * @see #natsSubscription_NextMsg()
    */
    Msg
    NextMsg(int64_t timeout);

    /** \brief Unsubscribes.
    *
    * @see #natsSubscription_Unsubscribe()
    */
    void
    Unsubscribe();

    /** \brief Auto-Unsubscribes.
    *
    * @see #natsSubscription_AutoUnsubscribe()
    */
    void
    AutoUnsubscribe(int max);

    /** \brief Gets the number of pending messages.
    *
    * @see #natsSubscription_QueuedMsgs()
    */
    void
    QueuedMsgs(uint64_t * queuedMsgs);

    /** \brief Gets the subscription id.
    *
    * @see #natsSubscription_GetID()
    */
    int64_t
    GetID();

    /** \brief Gets the subject name.
    *
    * @see #natsSubscription_GetSubject()
    */
    const char *
    GetSubject();

    /** \brief Sets the limit for pending messages and bytes.
    *
    * @see #natsSubscription_SetPendingLimits()
    */
    void
    SetPendingLimits(int msgLimit, int bytesLimit);

    /** \brief Returns the current limit for pending messages and bytes.
    *
    * @see #natsSubscription_GetPendingLimits()
    */
    void
    GetPendingLimits(int * msgLimit, int * bytesLimit);

    /** \brief Returns the number of pending messages and bytes.
    *
    * @see #natsSubscription_GetPending()
    */
    void
    GetPending(int * msgs, int * bytes);

    /** \brief Returns the number of delivered messages.
    *
    * @see #natsSubscription_GetDelivered()
    */
    void
    GetDelivered(int64_t * msgs);

    /** \brief Returns the number of dropped messages.
    *
    * @see #natsSubscription_GetDropped()
    */
    void
    GetDropped(int64_t * msgs);

    /** \brief Returns the maximum number of pending messages and bytes.
    *
    * @see #natsSubscription_GetMaxPending()
    */
    void
    GetMaxPending(int * msgs, int * bytes);

    /** \brief Clears the statistics regarding the maximum pending values.
    *
    * @see #natsSubscription_ClearMaxPending()
    */
    void
    ClearMaxPending();

    /** \brief Get various statistics from this subscription.
    *
    * @see #natsSubscription_GetStats()
    */
    void
    GetStats(int * pendingMsgs, int * pendingBytes, int * maxPendingMsgs, int * maxPendingBytes, int64_t * deliveredMsgs, int64_t * droppedMsgs);

    /** \brief Checks the validity of the subscription.
    *
    * @see #natsSubscription_IsValid()
    */
    bool
    IsValid();

    /** \brief Drains the subscription with a default timeout.
    *
    * @see #natsSubscription_Drain()
    */
    void
    Drain();

    /** \brief Drains the subscription with the specified timeout.
    *
    * @see #natsSubscription_DrainTimeout()
    */
    void
    DrainTimeout(int64_t timeout);

    /** \brief Blocks until the drain operation completes.
    *
    * @see #natsSubscription_WaitForDrainCompletion()
    */
    void
    WaitForDrainCompletion(int64_t timeout);

    /** \brief Returns the status of the drain after completion.
    *
    * @see #natsSubscription_DrainCompletionStatus()
    */
    void
    DrainCompletionStatus();

    /** \brief Sets a completion callback.
    *
    * @see #natsSubscription_SetOnCompleteCB()
    */
    void
    SetOnCompleteCB(natsOnCompleteCB cb, void * closure);

    template<typename T1, OnCompleteCB<T1> callback1> void
    SetOnCompleteCB(T1 * closure);

    /** \brief Destroys the subscription.
    *
    * @see #natsSubscription_Destroy()
    */
    ~Subscription();

    /** \brief Fetches messages for a pull subscription.
    *
    * @see #natsSubscription_Fetch()
    */
    void
    Fetch(natsMsgList * list, int batch, int64_t timeout, jsErrCode * errCode);

    /** \brief Fetches messages for a pull subscription with a complete request configuration
    *
    * @see #natsSubscription_FetchRequest()
    */
    void
    FetchRequest(natsMsgList * list, jsFetchRequest * request);

    /** \brief Returns the jsConsumerInfo associated with this subscription.
    *
    * @see #natsSubscription_GetConsumerInfo()
    */
    void
    GetConsumerInfo(jsConsumerInfo ** ci, jsOptions * opts, jsErrCode * errCode);

    /** \brief Returns the consumer sequence mismatch information.
    *
    * @see #natsSubscription_GetSequenceMismatch()
    */
    void
    GetSequenceMismatch(jsConsumerSequenceMismatch * csm);

    operator const natsSubscription * () const
    {
        return self;
    }

    operator natsSubscription * ()
    {
        return self;
    }

    [[nodiscard]] natsSubscription *
    Release()
    {
        natsSubscription * ret = self;
        self = nullptr;
        return ret;
    }
};

class Subscription::WithoutDestuction : public Subscription {
public:
    WithoutDestuction(natsSubscription * ptr) : Subscription(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

/** \brief A structure holding a subject, optional reply and payload.
 *
 * #natsMsg is a structure used by Subscribers and
 * #natsConnection_PublishMsg().
 */
class Msg {
    class WithoutDestuction;
    friend class Connection;
    friend class Statistics;
    friend class Subscription;
    friend class Options;
    friend class Inbox;
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(natsConnection * nc, natsSubscription * sub, natsMsg * msg, void * closure);
    template<typename T, ConnectionHandler<T> callback> friend void
    ConnectionHandlerCallback(natsConnection * nc, void * closure);
    template<typename T, ErrHandler<T> callback> friend void
    ErrHandlerCallback(natsConnection * nc, natsSubscription * subscription, natsStatus err, void * closure);
    template<typename T, UserJWTHandler<T> callback> friend natsStatus
    UserJWTHandlerCallback(char ** userJWT, char ** customErrTxt, void * closure);
    template<typename T, SignatureHandler<T> callback> friend natsStatus
    SignatureHandlerCallback(char ** customErrTxt, unsigned char ** signature, int * signatureLength, const char * nonce, void * closure);
    template<typename T, TokenHandler<T> callback> friend void
    TokenHandlerCallback(void * closure);
    template<typename T, OnCompleteCB<T> callback> friend void
    OnCompleteCBCallback(void * closure);
    template<typename T, CustomReconnectDelayHandler<T> callback> friend int64_t
    CustomReconnectDelayHandlerCallback(natsConnection * nc, int attempts, void * closure);
    natsMsg * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit Msg(natsMsg* ptr) : self(ptr)
    {
    }

    Msg(Msg&& other) : self(other.Release())
    {
    }

    /** \brief Destroys this list of messages.
    *
    * @see #natsMsgList_Destroy()
    */
    ~Msg();

    /** \brief Creates a #natsMsg object.
    *
    * @see #natsMsg_Create()
    */
    Msg(const char * subj, const char * reply, const char * data, int dataLen);

    /** \brief Returns the subject set in this message.
    *
    * @see #natsMsg_GetSubject()
    */
    const char *
    GetSubject() const;

    /** \brief Returns the reply set in this message.
    *
    * @see #natsMsg_GetReply()
    */
    const char *
    GetReply() const;

    /** \brief Returns the message payload.
    *
    * @see #natsMsg_GetData()
    */
    const char *
    GetData() const;

    /** \brief Returns the message length.
    *
    * @see #natsMsg_GetDataLength()
    */
    int
    GetDataLength() const;

    /** \brief Set the header entries associated with `key` to the single element `value`.
    *
    * @see #natsMsgHeader_Set()
    */
    void
    Set(const char * key, const char * value);

    /** \brief Add `value` to the header associated with `key`.
    *
    * @see #natsMsgHeader_Add()
    */
    void
    Add(const char * key, const char * value);

    /** \brief Get the header entry associated with `key`.
    *
    * @see #natsMsgHeader_Get()
    */
    void
    Get(const char * key, const char ** value);

    /** \brief Get all header values associated with `key`.
    *
    * @see #natsMsgHeader_Values()
    */
    void
    Values(const char * key, const char *** values, int * count);

    /** \brief Get all header keys.
    *
    * @see #natsMsgHeader_Keys()
    */
    void
    Keys(const char *** keys, int * count);

    /** \brief Delete the value(s) associated with `key`.
    *
    * @see #natsMsgHeader_Delete()
    */
    void
    Delete(const char * key);

    /** \brief Indicates if this message is a "no responders" message from the server.
    *
    * @see #natsMsg_IsNoResponders()
    */
    bool
    IsNoResponders();

    /** \brief Returns metadata from this JetStream message.
    *
    * @see #natsMsg_GetMetaData()
    */
    void
    GetMetaData(jsMsgMetaData ** new_meta);

    /** \brief Acknowledges a message.
    *
    * @see #natsMsg_Ack()
    */
    void
    Ack(jsOptions * opts);

    /** \brief Acknowledges a message and wait for a confirmation.
    *
    * @see #natsMsg_AckSync()
    */
    void
    AckSync(jsOptions * opts, jsErrCode * errCode);

    /** \brief Negatively acknowledges a message.
    *
    * @see #natsMsg_Nak()
    */
    void
    Nak(jsOptions * opts);

    /** \brief Negatively acknowledges a message.
    *
    * @see #natsMsg_NakWithDelay()
    */
    void
    NakWithDelay(int64_t delay, jsOptions * opts);

    /** \brief Resets redelivery timer on the server.
    *
    * @see #natsMsg_InProgress()
    */
    void
    InProgress(jsOptions * opts);

    /** \brief Abandon this message.
    *
    * @see #natsMsg_Term()
    */
    void
    Term(jsOptions * opts);

    /** \brief Returns the sequence number of this JetStream message.
    *
    * @see #natsMsg_GetSequence()
    */
    uint64_t
    GetSequence();

    /** \brief Returns the timestamp (in UTC) of this JetStream message.
    *
    * @see #natsMsg_GetTime()
    */
    int64_t
    GetTime();

    operator const natsMsg * () const
    {
        return self;
    }

    operator natsMsg * ()
    {
        return self;
    }

    [[nodiscard]] natsMsg *
    Release()
    {
        natsMsg * ret = self;
        self = nullptr;
        return ret;
    }
};

class Msg::WithoutDestuction : public Msg {
public:
    WithoutDestuction(natsMsg * ptr) : Msg(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

/** \brief Way to configure a #natsConnection.
 *
 * Options can be used to create a customized #natsConnection.
 */
class Options {
    class WithoutDestuction;
    friend class Connection;
    friend class Statistics;
    friend class Subscription;
    friend class Msg;
    friend class Inbox;
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(natsConnection * nc, natsSubscription * sub, natsMsg * msg, void * closure);
    template<typename T, ConnectionHandler<T> callback> friend void
    ConnectionHandlerCallback(natsConnection * nc, void * closure);
    template<typename T, ErrHandler<T> callback> friend void
    ErrHandlerCallback(natsConnection * nc, natsSubscription * subscription, natsStatus err, void * closure);
    template<typename T, UserJWTHandler<T> callback> friend natsStatus
    UserJWTHandlerCallback(char ** userJWT, char ** customErrTxt, void * closure);
    template<typename T, SignatureHandler<T> callback> friend natsStatus
    SignatureHandlerCallback(char ** customErrTxt, unsigned char ** signature, int * signatureLength, const char * nonce, void * closure);
    template<typename T, TokenHandler<T> callback> friend void
    TokenHandlerCallback(void * closure);
    template<typename T, OnCompleteCB<T> callback> friend void
    OnCompleteCBCallback(void * closure);
    template<typename T, CustomReconnectDelayHandler<T> callback> friend int64_t
    CustomReconnectDelayHandlerCallback(natsConnection * nc, int attempts, void * closure);
    natsOptions * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit Options(natsOptions* ptr) : self(ptr)
    {
    }

    Options(Options&& other) : self(other.Release())
    {
    }

    /** \brief Creates a #natsOptions object.
    *
    * @see #natsOptions_Create()
    */
    Options();

    /** \brief None
    *
    * @see #natsOptions_SetURL()
    */
    void
    SetURL(const char * url);

    /** \brief Set the list of servers to try to (re)connect to.
    *
    * @see #natsOptions_SetServers()
    */
    void
    SetServers(const char ** servers, int serversCount);

    /** \brief Sets the user name/password to use when not specified in the URL.
    *
    * @see #natsOptions_SetUserInfo()
    */
    void
    SetUserInfo(const char * user, const char * password);

    /** \brief Sets the token to use when not specified in the URL.
    *
    * @see #natsOptions_SetToken()
    */
    void
    SetToken(const char * token);

    /** \brief Sets the tokenCb to use whenever a token is needed.
    *
    * @see #natsOptions_SetTokenHandler()
    */
    void
    SetTokenHandler(natsTokenHandler tokenCb, void * closure);

    template<typename T1, TokenHandler<T1> callback1> void
    SetTokenHandler(T1 * closure);

    /** \brief Indicate if the servers list should be randomized.
    *
    * @see #natsOptions_SetNoRandomize()
    */
    void
    SetNoRandomize(bool noRandomize);

    /** \brief Sets the (re)connect process timeout.
    *
    * @see #natsOptions_SetTimeout()
    */
    void
    SetTimeout(int64_t timeout);

    /** \brief Sets the name.
    *
    * @see #natsOptions_SetName()
    */
    void
    SetName(const char * name);

    /** \brief Sets the secure mode.
    *
    * @see #natsOptions_SetSecure()
    */
    void
    SetSecure(bool secure);

    /** \brief Loads the trusted CA certificates from a file.
    *
    * @see #natsOptions_LoadCATrustedCertificates()
    */
    void
    LoadCATrustedCertificates(const char * fileName);

    /** \brief Sets the trusted CA certificates from memory.
    *
    * @see #natsOptions_SetCATrustedCertificates()
    */
    void
    SetCATrustedCertificates(const char * certificates);

    /** \brief Loads the certificate chain from a file, using the given key.
    *
    * @see #natsOptions_LoadCertificatesChain()
    */
    void
    LoadCertificatesChain(const char * certsFileName, const char * keyFileName);

    /** \brief Sets the client certificate and key.
    *
    * @see #natsOptions_SetCertificatesChain()
    */
    void
    SetCertificatesChain(const char * cert, const char * key);

    /** \brief Sets the list of available ciphers.
    *
    * @see #natsOptions_SetCiphers()
    */
    void
    SetCiphers(const char * ciphers);

    /** \brief Sets the list of available ciphers for TLSv1.3.
    *
    * @see #natsOptions_SetCipherSuites()
    */
    void
    SetCipherSuites(const char * ciphers);

    /** \brief Sets the server certificate's expected hostname.
    *
    * @see #natsOptions_SetExpectedHostname()
    */
    void
    SetExpectedHostname(const char * hostname);

    /** \brief Switch server certificate verification.
    *
    * @see #natsOptions_SkipServerVerification()
    */
    void
    SkipServerVerification(bool skip);

    /** \brief Sets the verbose mode.
    *
    * @see #natsOptions_SetVerbose()
    */
    void
    SetVerbose(bool verbose);

    /** \brief Sets the pedantic mode.
    *
    * @see #natsOptions_SetPedantic()
    */
    void
    SetPedantic(bool pedantic);

    /** \brief Sets the ping interval.
    *
    * @see #natsOptions_SetPingInterval()
    */
    void
    SetPingInterval(int64_t interval);

    /** \brief Sets the limit of outstanding `PING`s without corresponding `PONG`s.
    *
    * @see #natsOptions_SetMaxPingsOut()
    */
    void
    SetMaxPingsOut(int maxPingsOut);

    /** \brief Sets the size of the internal read/write buffers.
    *
    * @see #natsOptions_SetIOBufSize()
    */
    void
    SetIOBufSize(int ioBufSize);

    /** \brief Indicates if the connection will be allowed to reconnect.
    *
    * @see #natsOptions_SetAllowReconnect()
    */
    void
    SetAllowReconnect(bool allow);

    /** \brief Sets the maximum number of reconnect attempts.
    *
    * @see #natsOptions_SetMaxReconnect()
    */
    void
    SetMaxReconnect(int maxReconnect);

    /** \brief Sets the time between reconnect attempts.
    *
    * @see #natsOptions_SetReconnectWait()
    */
    void
    SetReconnectWait(int64_t reconnectWait);

    /** \brief Set the upper bound of a random delay added to reconnect wait.
    *
    * @see #natsOptions_SetReconnectJitter()
    */
    void
    SetReconnectJitter(int64_t jitter, int64_t jitterTLS);

    /** \brief Sets the handler to invoke when the library needs to wait before the next reconnect attempts.
    *
    * @see #natsOptions_SetCustomReconnectDelay()
    */
    void
    SetCustomReconnectDelay(natsCustomReconnectDelayHandler cb, void * closure);

    template<typename T1, CustomReconnectDelayHandler<T1> callback1> void
    SetCustomReconnectDelay(T1 * closure);

    /** \brief Sets the size of the backing buffer used during reconnect.
    *
    * @see #natsOptions_SetReconnectBufSize()
    */
    void
    SetReconnectBufSize(int reconnectBufSize);

    /** \brief Sets the maximum number of pending messages per subscription.
    *
    * @see #natsOptions_SetMaxPendingMsgs()
    */
    void
    SetMaxPendingMsgs(int maxPending);

    /** \brief Sets the error handler for asynchronous events.
    *
    * @see #natsOptions_SetErrorHandler()
    */
    void
    SetErrorHandler(natsErrHandler errHandler, void * closure);

    template<typename T1, ErrHandler<T1> callback1> void
    SetErrorHandler(T1 * closure);

    /** \brief Sets the callback to be invoked when a connection to a server is permanently lost.
    *
    * @see #natsOptions_SetClosedCB()
    */
    void
    SetClosedCB(natsConnectionHandler closedCb, void * closure);

    template<typename T1, ConnectionHandler<T1> callback1> void
    SetClosedCB(T1 * closure);

    /** \brief Sets the callback to be invoked when the connection to a server is lost.
    *
    * @see #natsOptions_SetDisconnectedCB()
    */
    void
    SetDisconnectedCB(natsConnectionHandler disconnectedCb, void * closure);

    template<typename T1, ConnectionHandler<T1> callback1> void
    SetDisconnectedCB(T1 * closure);

    /** \brief Sets the callback to be invoked when the connection has reconnected.
    *
    * @see #natsOptions_SetReconnectedCB()
    */
    void
    SetReconnectedCB(natsConnectionHandler reconnectedCb, void * closure);

    template<typename T1, ConnectionHandler<T1> callback1> void
    SetReconnectedCB(T1 * closure);

    /** \brief Sets the callback to be invoked when new servers are discovered.
    *
    * @see #natsOptions_SetDiscoveredServersCB()
    */
    void
    SetDiscoveredServersCB(natsConnectionHandler discoveredServersCb, void * closure);

    template<typename T1, ConnectionHandler<T1> callback1> void
    SetDiscoveredServersCB(T1 * closure);

    /** \brief Sets if the library should ignore or not discovered servers.
    *
    * @see #natsOptions_SetIgnoreDiscoveredServers()
    */
    void
    SetIgnoreDiscoveredServers(bool ignore);

    /** \brief Sets the callback to be invoked when server enters lame duck mode.
    *
    * @see #natsOptions_SetLameDuckModeCB()
    */
    void
    SetLameDuckModeCB(natsConnectionHandler lameDuckCb, void * closure);

    template<typename T1, ConnectionHandler<T1> callback1> void
    SetLameDuckModeCB(T1 * closure);

    /** \brief Sets the external event loop and associated callbacks.
    *
    * @see #natsOptions_SetEventLoop()
    */
    void
    SetEventLoop(void * loop, natsEvLoop_Attach attachCb, natsEvLoop_ReadAddRemove readCb, natsEvLoop_WriteAddRemove writeCb, natsEvLoop_Detach detachCb);

    /** \brief Switch on/off the use of a central message delivery thread pool.
    *
    * @see #natsOptions_UseGlobalMessageDelivery()
    */
    void
    UseGlobalMessageDelivery(bool global);

    /** \brief Dictates the order in which host name are resolved during connect.
    *
    * @see #natsOptions_IPResolutionOrder()
    */
    void
    IPResolutionOrder(int order);

    /** \brief Sets if Publish calls should send data right away.
    *
    * @see #natsOptions_SetSendAsap()
    */
    void
    SetSendAsap(bool sendAsap);

    /** \brief Switches the use of old style requests.
    *
    * @see #natsOptions_UseOldRequestStyle()
    */
    void
    UseOldRequestStyle(bool useOldStyle);

    /** \brief Fails pending requests on disconnect event.
    *
    * @see #natsOptions_SetFailRequestsOnDisconnect()
    */
    void
    SetFailRequestsOnDisconnect(bool failRequests);

    /** \brief Sets if connection receives its own messages.
    *
    * @see #natsOptions_SetNoEcho()
    */
    void
    SetNoEcho(bool noEcho);

    /** \brief Indicates if initial connect failure should be retried or not.
    *
    * @see #natsOptions_SetRetryOnFailedConnect()
    */
    void
    SetRetryOnFailedConnect(bool retry, natsConnectionHandler connectedCb, void * closure);

    template<typename T1, ConnectionHandler<T1> callback1> void
    SetRetryOnFailedConnect(bool retry, T1 * closure);

    /** \brief Sets the callbacks to fetch user JWT and sign server's nonce.
    *
    * @see #natsOptions_SetUserCredentialsCallbacks()
    */
    void
    SetUserCredentialsCallbacks(natsUserJWTHandler ujwtCB, void * ujwtClosure, natsSignatureHandler sigCB, void * sigClosure);

    template<typename T1, UserJWTHandler<T1> callback1, typename T2, SignatureHandler<T2> callback2> void
    SetUserCredentialsCallbacks(T1 * ujwtClosure, T2 * sigClosure);

    /** \brief Sets the file(s) to use to fetch user JWT and seed required to sign nonce.
    *
    * @see #natsOptions_SetUserCredentialsFromFiles()
    */
    void
    SetUserCredentialsFromFiles(const char * userOrChainedFile, const char * seedFile);

    /** \brief Sets the NKey public key and signature callback.
    *
    * @see #natsOptions_SetNKey()
    */
    void
    SetNKey(const char * pubKey, natsSignatureHandler sigCB, void * sigClosure);

    template<typename T1, SignatureHandler<T1> callback1> void
    SetNKey(const char * pubKey, T1 * sigClosure);

    /** \brief Sets the NKey public key and its seed file.
    *
    * @see #natsOptions_SetNKeyFromSeed()
    */
    void
    SetNKeyFromSeed(const char * pubKey, const char * seedFile);

    /** \brief Sets the write deadline.
    *
    * @see #natsOptions_SetWriteDeadline()
    */
    void
    SetWriteDeadline(int64_t deadline);

    /** \brief Enable/Disable the "no responders" feature.
    *
    * @see #natsOptions_DisableNoResponders()
    */
    void
    DisableNoResponders(bool disabled);

    /** \brief Sets a custom inbox prefix
    *
    * @see #natsOptions_SetCustomInboxPrefix()
    */
    void
    SetCustomInboxPrefix(const char * inboxPrefix);

    /** \brief Destroys a #natsOptions object.
    *
    * @see #natsOptions_Destroy()
    */
    ~Options();

    operator const natsOptions * () const
    {
        return self;
    }

    operator natsOptions * ()
    {
        return self;
    }

    [[nodiscard]] natsOptions *
    Release()
    {
        natsOptions * ret = self;
        self = nullptr;
        return ret;
    }
};

class Options::WithoutDestuction : public Options {
public:
    WithoutDestuction(natsOptions * ptr) : Options(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

/** \brief Unique subject often used for point-to-point communication.
 *
 * This can be used as the reply for a request. Inboxes are meant to be
 * unique so that replies can be sent to a specific subscriber. That
 * being said, inboxes can be shared across multiple subscribers if
 * desired.
 */
class Inbox {
    class WithoutDestuction;
    friend class Connection;
    friend class Statistics;
    friend class Subscription;
    friend class Msg;
    friend class Options;
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(natsConnection * nc, natsSubscription * sub, natsMsg * msg, void * closure);
    template<typename T, ConnectionHandler<T> callback> friend void
    ConnectionHandlerCallback(natsConnection * nc, void * closure);
    template<typename T, ErrHandler<T> callback> friend void
    ErrHandlerCallback(natsConnection * nc, natsSubscription * subscription, natsStatus err, void * closure);
    template<typename T, UserJWTHandler<T> callback> friend natsStatus
    UserJWTHandlerCallback(char ** userJWT, char ** customErrTxt, void * closure);
    template<typename T, SignatureHandler<T> callback> friend natsStatus
    SignatureHandlerCallback(char ** customErrTxt, unsigned char ** signature, int * signatureLength, const char * nonce, void * closure);
    template<typename T, TokenHandler<T> callback> friend void
    TokenHandlerCallback(void * closure);
    template<typename T, OnCompleteCB<T> callback> friend void
    OnCompleteCBCallback(void * closure);
    template<typename T, CustomReconnectDelayHandler<T> callback> friend int64_t
    CustomReconnectDelayHandlerCallback(natsConnection * nc, int attempts, void * closure);
    natsInbox * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit Inbox(natsInbox* ptr) : self(ptr)
    {
    }

    Inbox(Inbox&& other) : self(other.Release())
    {
    }

    /** \brief Creates an inbox.
    *
    * @see #natsInbox_Create()
    */
    Inbox();

    /** \brief Destroys the inbox.
    *
    * @see #natsInbox_Destroy()
    */
    ~Inbox();

    operator const natsInbox * () const
    {
        return self;
    }

    operator natsInbox * ()
    {
        return self;
    }

    [[nodiscard]] natsInbox *
    Release()
    {
        natsInbox * ret = self;
        self = nullptr;
        return ret;
    }
};

class Inbox::WithoutDestuction : public Inbox {
public:
    WithoutDestuction(natsInbox * ptr) : Inbox(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

/** \brief Initializes the library.
*
* @see #nats_Open()
*/
inline void
Open(int64_t lockSpinCount)
{
    return Exception::CheckResult(nats_Open(lockSpinCount));
}

/** \brief Returns the Library's version.
*
* @see #nats_GetVersion()
*/
inline const char *
GetVersion()
{
    return nats_GetVersion();
}

/** \brief Returns the Library's version as a number.
*
* @see #nats_GetVersionNumber()
*/
inline uint32_t
GetVersionNumber()
{
    return nats_GetVersionNumber();
}

/** \brief Check that the header is compatible with the library.
*
* @see #nats_CheckCompatibility()
*/
inline bool
CheckCompatibility()
{
    return nats_CheckCompatibility();
}

/** \brief Gives the current time in milliseconds.
*
* @see #nats_Now()
*/
inline int64_t
Now()
{
    return nats_Now();
}

/** \brief Gives the current time in nanoseconds.
*
* @see #nats_NowInNanoSeconds()
*/
inline int64_t
NowInNanoSeconds()
{
    return nats_NowInNanoSeconds();
}

/** \brief Sleeps for a given number of milliseconds.
*
* @see #nats_Sleep()
*/
inline void
Sleep(int64_t sleepTime)
{
    return nats_Sleep(sleepTime);
}

/** \brief Returns the calling thread's last known error.
*
* @see #nats_GetLastError()
*/
inline const char *
GetLastError(natsStatus * status)
{
    return nats_GetLastError(status);
}

/** \brief Returns the calling thread's last known error stack.
*
* @see #nats_GetLastErrorStack()
*/
inline void
GetLastErrorStack(char * buffer, size_t bufLen)
{
    return Exception::CheckResult(nats_GetLastErrorStack(buffer, bufLen));
}

/** \brief Prints the calling thread's last known error stack into the file.
*
* @see #nats_PrintLastErrorStack()
*/
inline void
PrintLastErrorStack(FILE * file)
{
    return nats_PrintLastErrorStack(file);
}

/** \brief Sets the maximum size of the global message delivery thread pool.
*
* @see #nats_SetMessageDeliveryPoolSize()
*/
inline void
SetMessageDeliveryPoolSize(int max)
{
    return Exception::CheckResult(nats_SetMessageDeliveryPoolSize(max));
}

/** \brief Release thread-local memory possibly allocated by the library.
*
* @see #nats_ReleaseThreadMemory()
*/
inline void
ReleaseThreadMemory()
{
    return nats_ReleaseThreadMemory();
}

/** \brief Signs a given text using the provided private key.
*
* @see #nats_Sign()
*/
inline void
Sign(const char * encodedSeed, const char * input, unsigned char ** signature, int * signatureLength)
{
    return Exception::CheckResult(nats_Sign(encodedSeed, input, signature, signatureLength));
}

/** \brief Tear down the library.
*
* @see #nats_Close()
*/
inline void
Close()
{
    return nats_Close();
}

/** \brief Tear down the library and wait for all resources to be released.
*
* @see #nats_CloseAndWait()
*/
inline void
CloseAndWait(int64_t timeout)
{
    return Exception::CheckResult(nats_CloseAndWait(timeout));
}

/** \brief Get the text corresponding to a #natsStatus.
*
* @see #natsStatus_GetText()
*/
inline const char *
GetText(natsStatus s)
{
    return natsStatus_GetText(s);
}

inline Connection::Connection(natsOptions * options)
{
    Exception::CheckResult(natsConnection_Connect(&self, options));
}

inline void
Connection::ProcessReadEvent()
{
    return natsConnection_ProcessReadEvent(self);
}

inline void
Connection::ProcessWriteEvent()
{
    return natsConnection_ProcessWriteEvent(self);
}

inline Connection::Connection(const char * urls)
{
    Exception::CheckResult(natsConnection_ConnectTo(&self, urls));
}

inline bool
Connection::IsClosed()
{
    return natsConnection_IsClosed(self);
}

inline bool
Connection::IsReconnecting()
{
    return natsConnection_IsReconnecting(self);
}

inline bool
Connection::IsDraining()
{
    return natsConnection_IsDraining(self);
}

inline natsConnStatus
Connection::Status()
{
    return natsConnection_Status(self);
}

inline int
Connection::Buffered()
{
    return natsConnection_Buffered(self);
}

inline void
Connection::Flush()
{
    return Exception::CheckResult(natsConnection_Flush(self));
}

inline void
Connection::FlushTimeout(int64_t timeout)
{
    return Exception::CheckResult(natsConnection_FlushTimeout(self, timeout));
}

inline int64_t
Connection::GetMaxPayload()
{
    return natsConnection_GetMaxPayload(self);
}

inline void
Connection::GetStats(natsStatistics * stats)
{
    return Exception::CheckResult(natsConnection_GetStats(self, stats));
}

inline void
Connection::GetConnectedUrl(char * buffer, size_t bufferSize)
{
    return Exception::CheckResult(natsConnection_GetConnectedUrl(self, buffer, bufferSize));
}

inline void
Connection::GetConnectedServerId(char * buffer, size_t bufferSize)
{
    return Exception::CheckResult(natsConnection_GetConnectedServerId(self, buffer, bufferSize));
}

inline void
Connection::GetServers(char *** servers, int * count)
{
    return Exception::CheckResult(natsConnection_GetServers(self, servers, count));
}

inline void
Connection::GetDiscoveredServers(char *** servers, int * count)
{
    return Exception::CheckResult(natsConnection_GetDiscoveredServers(self, servers, count));
}

inline void
Connection::GetLastError(const char ** lastError)
{
    return Exception::CheckResult(natsConnection_GetLastError(self, lastError));
}

inline void
Connection::GetClientID(uint64_t * cid)
{
    return Exception::CheckResult(natsConnection_GetClientID(self, cid));
}

inline void
Connection::Drain()
{
    return Exception::CheckResult(natsConnection_Drain(self));
}

inline void
Connection::DrainTimeout(int64_t timeout)
{
    return Exception::CheckResult(natsConnection_DrainTimeout(self, timeout));
}

inline void
Connection::Sign(const unsigned char * message, int messageLen, unsigned char sig[64])
{
    return Exception::CheckResult(natsConnection_Sign(self, message, messageLen, sig));
}

inline void
Connection::GetClientIP(char ** ip)
{
    return Exception::CheckResult(natsConnection_GetClientIP(self, ip));
}

inline void
Connection::GetRTT(int64_t * rtt)
{
    return Exception::CheckResult(natsConnection_GetRTT(self, rtt));
}

inline void
Connection::HasHeaderSupport()
{
    return Exception::CheckResult(natsConnection_HasHeaderSupport(self));
}

inline void
Connection::GetLocalIPAndPort(char ** ip, int * port)
{
    return Exception::CheckResult(natsConnection_GetLocalIPAndPort(self, ip, port));
}

inline void
Connection::Close()
{
    return natsConnection_Close(self);
}

inline Connection::~Connection()
{
    natsConnection_Destroy(self);
}

inline void
Connection::Publish(const char * subj, const void * data, int dataLen)
{
    return Exception::CheckResult(natsConnection_Publish(self, subj, data, dataLen));
}

inline void
Connection::PublishString(const char * subj, const char * str)
{
    return Exception::CheckResult(natsConnection_PublishString(self, subj, str));
}

inline void
Connection::PublishMsg(natsMsg * msg)
{
    return Exception::CheckResult(natsConnection_PublishMsg(self, msg));
}

inline void
Connection::PublishRequest(const char * subj, const char * reply, const void * data, int dataLen)
{
    return Exception::CheckResult(natsConnection_PublishRequest(self, subj, reply, data, dataLen));
}

inline void
Connection::PublishRequestString(const char * subj, const char * reply, const char * str)
{
    return Exception::CheckResult(natsConnection_PublishRequestString(self, subj, reply, str));
}

inline Msg
Connection::Request(const char * subj, const void * data, int dataLen, int64_t timeout)
{
    Msg ret(nullptr);
    Exception::CheckResult(natsConnection_Request(&ret.self, self, subj, data, dataLen, timeout));
    return ret;
}

inline Msg
Connection::RequestString(const char * subj, const char * str, int64_t timeout)
{
    Msg ret(nullptr);
    Exception::CheckResult(natsConnection_RequestString(&ret.self, self, subj, str, timeout));
    return ret;
}

inline Msg
Connection::RequestMsg(natsMsg * requestMsg, int64_t timeout)
{
    Msg ret(nullptr);
    Exception::CheckResult(natsConnection_RequestMsg(&ret.self, self, requestMsg, timeout));
    return ret;
}

inline Subscription
Connection::Subscribe(const char * subject, natsMsgHandler cb, void * cbClosure)
{
    Subscription ret(nullptr);
    Exception::CheckResult(natsConnection_Subscribe(&ret.self, self, subject, cb, cbClosure));
    return ret;
}

template<typename T1, MsgHandler<T1> callback1> inline Subscription
Connection::Subscribe(const char * subject, T1 * cbClosure)
{
    return Subscribe(subject, &MsgHandlerCallback<T1, callback1>, cbClosure);
}

inline Subscription
Connection::SubscribeTimeout(const char * subject, int64_t timeout, natsMsgHandler cb, void * cbClosure)
{
    Subscription ret(nullptr);
    Exception::CheckResult(natsConnection_SubscribeTimeout(&ret.self, self, subject, timeout, cb, cbClosure));
    return ret;
}

template<typename T1, MsgHandler<T1> callback1> inline Subscription
Connection::SubscribeTimeout(const char * subject, int64_t timeout, T1 * cbClosure)
{
    return SubscribeTimeout(subject, timeout, &MsgHandlerCallback<T1, callback1>, cbClosure);
}

inline Subscription
Connection::SubscribeSync(const char * subject)
{
    Subscription ret(nullptr);
    Exception::CheckResult(natsConnection_SubscribeSync(&ret.self, self, subject));
    return ret;
}

inline Subscription
Connection::QueueSubscribe(const char * subject, const char * queueGroup, natsMsgHandler cb, void * cbClosure)
{
    Subscription ret(nullptr);
    Exception::CheckResult(natsConnection_QueueSubscribe(&ret.self, self, subject, queueGroup, cb, cbClosure));
    return ret;
}

template<typename T1, MsgHandler<T1> callback1> inline Subscription
Connection::QueueSubscribe(const char * subject, const char * queueGroup, T1 * cbClosure)
{
    return QueueSubscribe(subject, queueGroup, &MsgHandlerCallback<T1, callback1>, cbClosure);
}

inline Subscription
Connection::QueueSubscribeTimeout(const char * subject, const char * queueGroup, int64_t timeout, natsMsgHandler cb, void * cbClosure)
{
    Subscription ret(nullptr);
    Exception::CheckResult(natsConnection_QueueSubscribeTimeout(&ret.self, self, subject, queueGroup, timeout, cb, cbClosure));
    return ret;
}

template<typename T1, MsgHandler<T1> callback1> inline Subscription
Connection::QueueSubscribeTimeout(const char * subject, const char * queueGroup, int64_t timeout, T1 * cbClosure)
{
    return QueueSubscribeTimeout(subject, queueGroup, timeout, &MsgHandlerCallback<T1, callback1>, cbClosure);
}

inline Subscription
Connection::QueueSubscribeSync(const char * subject, const char * queueGroup)
{
    Subscription ret(nullptr);
    Exception::CheckResult(natsConnection_QueueSubscribeSync(&ret.self, self, subject, queueGroup));
    return ret;
}

inline void
Connection::JetStream(jsCtx ** js, jsOptions * opts)
{
    return Exception::CheckResult(natsConnection_JetStream(js, self, opts));
}

inline Statistics::Statistics()
{
    Exception::CheckResult(natsStatistics_Create(&self));
}

inline void
Statistics::GetCounts(uint64_t * inMsgs, uint64_t * inBytes, uint64_t * outMsgs, uint64_t * outBytes, uint64_t * reconnects) const
{
    return Exception::CheckResult(natsStatistics_GetCounts(self, inMsgs, inBytes, outMsgs, outBytes, reconnects));
}

inline Statistics::~Statistics()
{
    natsStatistics_Destroy(self);
}

inline void
Subscription::NoDeliveryDelay()
{
    return Exception::CheckResult(natsSubscription_NoDeliveryDelay(self));
}

inline Msg
Subscription::NextMsg(int64_t timeout)
{
    Msg ret(nullptr);
    Exception::CheckResult(natsSubscription_NextMsg(&ret.self, self, timeout));
    return ret;
}

inline void
Subscription::Unsubscribe()
{
    return Exception::CheckResult(natsSubscription_Unsubscribe(self));
}

inline void
Subscription::AutoUnsubscribe(int max)
{
    return Exception::CheckResult(natsSubscription_AutoUnsubscribe(self, max));
}

inline void
Subscription::QueuedMsgs(uint64_t * queuedMsgs)
{
    return Exception::CheckResult(natsSubscription_QueuedMsgs(self, queuedMsgs));
}

inline int64_t
Subscription::GetID()
{
    return natsSubscription_GetID(self);
}

inline const char *
Subscription::GetSubject()
{
    return natsSubscription_GetSubject(self);
}

inline void
Subscription::SetPendingLimits(int msgLimit, int bytesLimit)
{
    return Exception::CheckResult(natsSubscription_SetPendingLimits(self, msgLimit, bytesLimit));
}

inline void
Subscription::GetPendingLimits(int * msgLimit, int * bytesLimit)
{
    return Exception::CheckResult(natsSubscription_GetPendingLimits(self, msgLimit, bytesLimit));
}

inline void
Subscription::GetPending(int * msgs, int * bytes)
{
    return Exception::CheckResult(natsSubscription_GetPending(self, msgs, bytes));
}

inline void
Subscription::GetDelivered(int64_t * msgs)
{
    return Exception::CheckResult(natsSubscription_GetDelivered(self, msgs));
}

inline void
Subscription::GetDropped(int64_t * msgs)
{
    return Exception::CheckResult(natsSubscription_GetDropped(self, msgs));
}

inline void
Subscription::GetMaxPending(int * msgs, int * bytes)
{
    return Exception::CheckResult(natsSubscription_GetMaxPending(self, msgs, bytes));
}

inline void
Subscription::ClearMaxPending()
{
    return Exception::CheckResult(natsSubscription_ClearMaxPending(self));
}

inline void
Subscription::GetStats(int * pendingMsgs, int * pendingBytes, int * maxPendingMsgs, int * maxPendingBytes, int64_t * deliveredMsgs, int64_t * droppedMsgs)
{
    return Exception::CheckResult(natsSubscription_GetStats(self, pendingMsgs, pendingBytes, maxPendingMsgs, maxPendingBytes, deliveredMsgs, droppedMsgs));
}

inline bool
Subscription::IsValid()
{
    return natsSubscription_IsValid(self);
}

inline void
Subscription::Drain()
{
    return Exception::CheckResult(natsSubscription_Drain(self));
}

inline void
Subscription::DrainTimeout(int64_t timeout)
{
    return Exception::CheckResult(natsSubscription_DrainTimeout(self, timeout));
}

inline void
Subscription::WaitForDrainCompletion(int64_t timeout)
{
    return Exception::CheckResult(natsSubscription_WaitForDrainCompletion(self, timeout));
}

inline void
Subscription::DrainCompletionStatus()
{
    return Exception::CheckResult(natsSubscription_DrainCompletionStatus(self));
}

inline void
Subscription::SetOnCompleteCB(natsOnCompleteCB cb, void * closure)
{
    return Exception::CheckResult(natsSubscription_SetOnCompleteCB(self, cb, closure));
}

template<typename T1, OnCompleteCB<T1> callback1> inline void
Subscription::SetOnCompleteCB(T1 * closure)
{
    return SetOnCompleteCB(&OnCompleteCBCallback<T1, callback1>, closure);
}

inline Subscription::~Subscription()
{
    natsSubscription_Destroy(self);
}

inline void
Subscription::Fetch(natsMsgList * list, int batch, int64_t timeout, jsErrCode * errCode)
{
    return Exception::CheckResult(natsSubscription_Fetch(list, self, batch, timeout, errCode));
}

inline void
Subscription::FetchRequest(natsMsgList * list, jsFetchRequest * request)
{
    return Exception::CheckResult(natsSubscription_FetchRequest(list, self, request));
}

inline void
Subscription::GetConsumerInfo(jsConsumerInfo ** ci, jsOptions * opts, jsErrCode * errCode)
{
    return Exception::CheckResult(natsSubscription_GetConsumerInfo(ci, self, opts, errCode));
}

inline void
Subscription::GetSequenceMismatch(jsConsumerSequenceMismatch * csm)
{
    return Exception::CheckResult(natsSubscription_GetSequenceMismatch(csm, self));
}

inline Msg::~Msg()
{
    natsMsgList_Destroy(self);
}

inline Msg::Msg(const char * subj, const char * reply, const char * data, int dataLen)
{
    Exception::CheckResult(natsMsg_Create(&self, subj, reply, data, dataLen));
}

inline const char *
Msg::GetSubject() const
{
    return natsMsg_GetSubject(self);
}

inline const char *
Msg::GetReply() const
{
    return natsMsg_GetReply(self);
}

inline const char *
Msg::GetData() const
{
    return natsMsg_GetData(self);
}

inline int
Msg::GetDataLength() const
{
    return natsMsg_GetDataLength(self);
}

inline void
Msg::Set(const char * key, const char * value)
{
    return Exception::CheckResult(natsMsgHeader_Set(self, key, value));
}

inline void
Msg::Add(const char * key, const char * value)
{
    return Exception::CheckResult(natsMsgHeader_Add(self, key, value));
}

inline void
Msg::Get(const char * key, const char ** value)
{
    return Exception::CheckResult(natsMsgHeader_Get(self, key, value));
}

inline void
Msg::Values(const char * key, const char *** values, int * count)
{
    return Exception::CheckResult(natsMsgHeader_Values(self, key, values, count));
}

inline void
Msg::Keys(const char *** keys, int * count)
{
    return Exception::CheckResult(natsMsgHeader_Keys(self, keys, count));
}

inline void
Msg::Delete(const char * key)
{
    return Exception::CheckResult(natsMsgHeader_Delete(self, key));
}

inline bool
Msg::IsNoResponders()
{
    return natsMsg_IsNoResponders(self);
}

inline void
Msg::GetMetaData(jsMsgMetaData ** new_meta)
{
    return Exception::CheckResult(natsMsg_GetMetaData(new_meta, self));
}

inline void
Msg::Ack(jsOptions * opts)
{
    return Exception::CheckResult(natsMsg_Ack(self, opts));
}

inline void
Msg::AckSync(jsOptions * opts, jsErrCode * errCode)
{
    return Exception::CheckResult(natsMsg_AckSync(self, opts, errCode));
}

inline void
Msg::Nak(jsOptions * opts)
{
    return Exception::CheckResult(natsMsg_Nak(self, opts));
}

inline void
Msg::NakWithDelay(int64_t delay, jsOptions * opts)
{
    return Exception::CheckResult(natsMsg_NakWithDelay(self, delay, opts));
}

inline void
Msg::InProgress(jsOptions * opts)
{
    return Exception::CheckResult(natsMsg_InProgress(self, opts));
}

inline void
Msg::Term(jsOptions * opts)
{
    return Exception::CheckResult(natsMsg_Term(self, opts));
}

inline uint64_t
Msg::GetSequence()
{
    return natsMsg_GetSequence(self);
}

inline int64_t
Msg::GetTime()
{
    return natsMsg_GetTime(self);
}

inline Options::Options()
{
    Exception::CheckResult(natsOptions_Create(&self));
}

inline void
Options::SetURL(const char * url)
{
    return Exception::CheckResult(natsOptions_SetURL(self, url));
}

inline void
Options::SetServers(const char ** servers, int serversCount)
{
    return Exception::CheckResult(natsOptions_SetServers(self, servers, serversCount));
}

inline void
Options::SetUserInfo(const char * user, const char * password)
{
    return Exception::CheckResult(natsOptions_SetUserInfo(self, user, password));
}

inline void
Options::SetToken(const char * token)
{
    return Exception::CheckResult(natsOptions_SetToken(self, token));
}

inline void
Options::SetTokenHandler(natsTokenHandler tokenCb, void * closure)
{
    return Exception::CheckResult(natsOptions_SetTokenHandler(self, tokenCb, closure));
}

template<typename T1, TokenHandler<T1> callback1> inline void
Options::SetTokenHandler(T1 * closure)
{
    return SetTokenHandler(&TokenHandlerCallback<T1, callback1>, closure);
}

inline void
Options::SetNoRandomize(bool noRandomize)
{
    return Exception::CheckResult(natsOptions_SetNoRandomize(self, noRandomize));
}

inline void
Options::SetTimeout(int64_t timeout)
{
    return Exception::CheckResult(natsOptions_SetTimeout(self, timeout));
}

inline void
Options::SetName(const char * name)
{
    return Exception::CheckResult(natsOptions_SetName(self, name));
}

inline void
Options::SetSecure(bool secure)
{
    return Exception::CheckResult(natsOptions_SetSecure(self, secure));
}

inline void
Options::LoadCATrustedCertificates(const char * fileName)
{
    return Exception::CheckResult(natsOptions_LoadCATrustedCertificates(self, fileName));
}

inline void
Options::SetCATrustedCertificates(const char * certificates)
{
    return Exception::CheckResult(natsOptions_SetCATrustedCertificates(self, certificates));
}

inline void
Options::LoadCertificatesChain(const char * certsFileName, const char * keyFileName)
{
    return Exception::CheckResult(natsOptions_LoadCertificatesChain(self, certsFileName, keyFileName));
}

inline void
Options::SetCertificatesChain(const char * cert, const char * key)
{
    return Exception::CheckResult(natsOptions_SetCertificatesChain(self, cert, key));
}

inline void
Options::SetCiphers(const char * ciphers)
{
    return Exception::CheckResult(natsOptions_SetCiphers(self, ciphers));
}

inline void
Options::SetCipherSuites(const char * ciphers)
{
    return Exception::CheckResult(natsOptions_SetCipherSuites(self, ciphers));
}

inline void
Options::SetExpectedHostname(const char * hostname)
{
    return Exception::CheckResult(natsOptions_SetExpectedHostname(self, hostname));
}

inline void
Options::SkipServerVerification(bool skip)
{
    return Exception::CheckResult(natsOptions_SkipServerVerification(self, skip));
}

inline void
Options::SetVerbose(bool verbose)
{
    return Exception::CheckResult(natsOptions_SetVerbose(self, verbose));
}

inline void
Options::SetPedantic(bool pedantic)
{
    return Exception::CheckResult(natsOptions_SetPedantic(self, pedantic));
}

inline void
Options::SetPingInterval(int64_t interval)
{
    return Exception::CheckResult(natsOptions_SetPingInterval(self, interval));
}

inline void
Options::SetMaxPingsOut(int maxPingsOut)
{
    return Exception::CheckResult(natsOptions_SetMaxPingsOut(self, maxPingsOut));
}

inline void
Options::SetIOBufSize(int ioBufSize)
{
    return Exception::CheckResult(natsOptions_SetIOBufSize(self, ioBufSize));
}

inline void
Options::SetAllowReconnect(bool allow)
{
    return Exception::CheckResult(natsOptions_SetAllowReconnect(self, allow));
}

inline void
Options::SetMaxReconnect(int maxReconnect)
{
    return Exception::CheckResult(natsOptions_SetMaxReconnect(self, maxReconnect));
}

inline void
Options::SetReconnectWait(int64_t reconnectWait)
{
    return Exception::CheckResult(natsOptions_SetReconnectWait(self, reconnectWait));
}

inline void
Options::SetReconnectJitter(int64_t jitter, int64_t jitterTLS)
{
    return Exception::CheckResult(natsOptions_SetReconnectJitter(self, jitter, jitterTLS));
}

inline void
Options::SetCustomReconnectDelay(natsCustomReconnectDelayHandler cb, void * closure)
{
    return Exception::CheckResult(natsOptions_SetCustomReconnectDelay(self, cb, closure));
}

template<typename T1, CustomReconnectDelayHandler<T1> callback1> inline void
Options::SetCustomReconnectDelay(T1 * closure)
{
    return SetCustomReconnectDelay(&CustomReconnectDelayHandlerCallback<T1, callback1>, closure);
}

inline void
Options::SetReconnectBufSize(int reconnectBufSize)
{
    return Exception::CheckResult(natsOptions_SetReconnectBufSize(self, reconnectBufSize));
}

inline void
Options::SetMaxPendingMsgs(int maxPending)
{
    return Exception::CheckResult(natsOptions_SetMaxPendingMsgs(self, maxPending));
}

inline void
Options::SetErrorHandler(natsErrHandler errHandler, void * closure)
{
    return Exception::CheckResult(natsOptions_SetErrorHandler(self, errHandler, closure));
}

template<typename T1, ErrHandler<T1> callback1> inline void
Options::SetErrorHandler(T1 * closure)
{
    return SetErrorHandler(&ErrHandlerCallback<T1, callback1>, closure);
}

inline void
Options::SetClosedCB(natsConnectionHandler closedCb, void * closure)
{
    return Exception::CheckResult(natsOptions_SetClosedCB(self, closedCb, closure));
}

template<typename T1, ConnectionHandler<T1> callback1> inline void
Options::SetClosedCB(T1 * closure)
{
    return SetClosedCB(&ConnectionHandlerCallback<T1, callback1>, closure);
}

inline void
Options::SetDisconnectedCB(natsConnectionHandler disconnectedCb, void * closure)
{
    return Exception::CheckResult(natsOptions_SetDisconnectedCB(self, disconnectedCb, closure));
}

template<typename T1, ConnectionHandler<T1> callback1> inline void
Options::SetDisconnectedCB(T1 * closure)
{
    return SetDisconnectedCB(&ConnectionHandlerCallback<T1, callback1>, closure);
}

inline void
Options::SetReconnectedCB(natsConnectionHandler reconnectedCb, void * closure)
{
    return Exception::CheckResult(natsOptions_SetReconnectedCB(self, reconnectedCb, closure));
}

template<typename T1, ConnectionHandler<T1> callback1> inline void
Options::SetReconnectedCB(T1 * closure)
{
    return SetReconnectedCB(&ConnectionHandlerCallback<T1, callback1>, closure);
}

inline void
Options::SetDiscoveredServersCB(natsConnectionHandler discoveredServersCb, void * closure)
{
    return Exception::CheckResult(natsOptions_SetDiscoveredServersCB(self, discoveredServersCb, closure));
}

template<typename T1, ConnectionHandler<T1> callback1> inline void
Options::SetDiscoveredServersCB(T1 * closure)
{
    return SetDiscoveredServersCB(&ConnectionHandlerCallback<T1, callback1>, closure);
}

inline void
Options::SetIgnoreDiscoveredServers(bool ignore)
{
    return Exception::CheckResult(natsOptions_SetIgnoreDiscoveredServers(self, ignore));
}

inline void
Options::SetLameDuckModeCB(natsConnectionHandler lameDuckCb, void * closure)
{
    return Exception::CheckResult(natsOptions_SetLameDuckModeCB(self, lameDuckCb, closure));
}

template<typename T1, ConnectionHandler<T1> callback1> inline void
Options::SetLameDuckModeCB(T1 * closure)
{
    return SetLameDuckModeCB(&ConnectionHandlerCallback<T1, callback1>, closure);
}

inline void
Options::SetEventLoop(void * loop, natsEvLoop_Attach attachCb, natsEvLoop_ReadAddRemove readCb, natsEvLoop_WriteAddRemove writeCb, natsEvLoop_Detach detachCb)
{
    return Exception::CheckResult(natsOptions_SetEventLoop(self, loop, attachCb, readCb, writeCb, detachCb));
}

inline void
Options::UseGlobalMessageDelivery(bool global)
{
    return Exception::CheckResult(natsOptions_UseGlobalMessageDelivery(self, global));
}

inline void
Options::IPResolutionOrder(int order)
{
    return Exception::CheckResult(natsOptions_IPResolutionOrder(self, order));
}

inline void
Options::SetSendAsap(bool sendAsap)
{
    return Exception::CheckResult(natsOptions_SetSendAsap(self, sendAsap));
}

inline void
Options::UseOldRequestStyle(bool useOldStyle)
{
    return Exception::CheckResult(natsOptions_UseOldRequestStyle(self, useOldStyle));
}

inline void
Options::SetFailRequestsOnDisconnect(bool failRequests)
{
    return Exception::CheckResult(natsOptions_SetFailRequestsOnDisconnect(self, failRequests));
}

inline void
Options::SetNoEcho(bool noEcho)
{
    return Exception::CheckResult(natsOptions_SetNoEcho(self, noEcho));
}

inline void
Options::SetRetryOnFailedConnect(bool retry, natsConnectionHandler connectedCb, void * closure)
{
    return Exception::CheckResult(natsOptions_SetRetryOnFailedConnect(self, retry, connectedCb, closure));
}

template<typename T1, ConnectionHandler<T1> callback1> inline void
Options::SetRetryOnFailedConnect(bool retry, T1 * closure)
{
    return SetRetryOnFailedConnect(retry, &ConnectionHandlerCallback<T1, callback1>, closure);
}

inline void
Options::SetUserCredentialsCallbacks(natsUserJWTHandler ujwtCB, void * ujwtClosure, natsSignatureHandler sigCB, void * sigClosure)
{
    return Exception::CheckResult(natsOptions_SetUserCredentialsCallbacks(self, ujwtCB, ujwtClosure, sigCB, sigClosure));
}

template<typename T1, UserJWTHandler<T1> callback1, typename T2, SignatureHandler<T2> callback2> inline void
Options::SetUserCredentialsCallbacks(T1 * ujwtClosure, T2 * sigClosure)
{
    return SetUserCredentialsCallbacks(&UserJWTHandlerCallback<T1, callback1>, ujwtClosure, &SignatureHandlerCallback<T2, callback2>, sigClosure);
}

inline void
Options::SetUserCredentialsFromFiles(const char * userOrChainedFile, const char * seedFile)
{
    return Exception::CheckResult(natsOptions_SetUserCredentialsFromFiles(self, userOrChainedFile, seedFile));
}

inline void
Options::SetNKey(const char * pubKey, natsSignatureHandler sigCB, void * sigClosure)
{
    return Exception::CheckResult(natsOptions_SetNKey(self, pubKey, sigCB, sigClosure));
}

template<typename T1, SignatureHandler<T1> callback1> inline void
Options::SetNKey(const char * pubKey, T1 * sigClosure)
{
    return SetNKey(pubKey, &SignatureHandlerCallback<T1, callback1>, sigClosure);
}

inline void
Options::SetNKeyFromSeed(const char * pubKey, const char * seedFile)
{
    return Exception::CheckResult(natsOptions_SetNKeyFromSeed(self, pubKey, seedFile));
}

inline void
Options::SetWriteDeadline(int64_t deadline)
{
    return Exception::CheckResult(natsOptions_SetWriteDeadline(self, deadline));
}

inline void
Options::DisableNoResponders(bool disabled)
{
    return Exception::CheckResult(natsOptions_DisableNoResponders(self, disabled));
}

inline void
Options::SetCustomInboxPrefix(const char * inboxPrefix)
{
    return Exception::CheckResult(natsOptions_SetCustomInboxPrefix(self, inboxPrefix));
}

inline Options::~Options()
{
    natsOptions_Destroy(self);
}

inline Inbox::Inbox()
{
    Exception::CheckResult(natsInbox_Create(&self));
}

inline Inbox::~Inbox()
{
    natsInbox_Destroy(self);
}

template<typename T, MsgHandler<T> callback> void
MsgHandlerCallback(natsConnection * nc, natsSubscription * sub, natsMsg * msg, void * closure)
{
    Connection::WithoutDestuction nc_(nc);
    Subscription::WithoutDestuction sub_(sub);
    T * self = static_cast<T *>(closure);
    return (self->*callback)(nc_, sub_, Msg(msg));
}

template<typename T, ConnectionHandler<T> callback> void
ConnectionHandlerCallback(natsConnection * nc, void * closure)
{
    Connection::WithoutDestuction nc_(nc);
    T * self = static_cast<T *>(closure);
    return (self->*callback)(nc_);
}

template<typename T, ErrHandler<T> callback> void
ErrHandlerCallback(natsConnection * nc, natsSubscription * subscription, natsStatus err, void * closure)
{
    Connection::WithoutDestuction nc_(nc);
    Subscription::WithoutDestuction subscription_(subscription);
    T * self = static_cast<T *>(closure);
    return (self->*callback)(nc_, subscription_, err);
}

template<typename T, UserJWTHandler<T> callback> natsStatus
UserJWTHandlerCallback(char ** userJWT, char ** customErrTxt, void * closure)
{
    T * self = static_cast<T *>(closure);
    return (self->*callback)(userJWT, customErrTxt);
}

template<typename T, SignatureHandler<T> callback> natsStatus
SignatureHandlerCallback(char ** customErrTxt, unsigned char ** signature, int * signatureLength, const char * nonce, void * closure)
{
    T * self = static_cast<T *>(closure);
    return (self->*callback)(customErrTxt, signature, signatureLength, nonce);
}

template<typename T, TokenHandler<T> callback> void
TokenHandlerCallback(void * closure)
{
    T * self = static_cast<T *>(closure);
    return (self->*callback)();
}

template<typename T, OnCompleteCB<T> callback> void
OnCompleteCBCallback(void * closure)
{
    T * self = static_cast<T *>(closure);
    return (self->*callback)();
}

template<typename T, CustomReconnectDelayHandler<T> callback> int64_t
CustomReconnectDelayHandlerCallback(natsConnection * nc, int attempts, void * closure)
{
    Connection::WithoutDestuction nc_(nc);
    T * self = static_cast<T *>(closure);
    return (self->*callback)(nc_, attempts);
}

} // namespace nats


#if defined(NATS_HAS_STREAMING)
namespace stan {
class Connection;
class Subscription;
class Msg;
class ConnOptions;
class SubOptions;

/** \brief Callback used to notify of an asynchronous publish result.
 *
 * This is used for asynchronous publishing to provide status of the acknowledgment.
 * The function will be passed the GUID and any error state. No error means the
 * message was successfully received by NATS Streaming.
 *
 * @see stanConnection_PublishAsync()
 */
template<typename T>
using PubAckHandler = void (T::*)(const char * guid, const char * error);

template<typename T, PubAckHandler<T> callback> void
PubAckHandlerCallback(const char * guid, const char * error, void * closure);

/** \brief Callback used to deliver messages to the application.
 *
 * This is the callback that one provides when creating an asynchronous
 * subscription. The library will invoke this callback for each message
 * arriving through the subscription's connection.
 *
 * @see stanConnection_Subscribe()
 * @see stanConnection_QueueSubscribe()
 */
template<typename T>
using MsgHandler = void (T::*)(Connection &, Subscription &, const char * channel, Msg &);

template<typename T, MsgHandler<T> callback> void
MsgHandlerCallback(stanConnection * sc, stanSubscription * sub, const char * channel, stanMsg * msg, void * closure);

/** \brief Callback used to notify the user of the permanent loss of the connection.
 *
 * This callback is used to notify the user that the connection to the Streaming
 * server is permanently lost.
 *
 */
template<typename T>
using ConnectionLostHandler = void (T::*)(Connection &, const char * errorTxt);

template<typename T, ConnectionLostHandler<T> callback> void
ConnectionLostHandlerCallback(stanConnection * sc, const char * errorTxt, void * closure);
using nats::Exception;

/** \brief A connection to a `NATS Streaming Server`.
 *
 * A #stanConnection represents a connection to a `NATS Streaming Server`.
 */
class Connection {
    class WithoutDestuction;
    friend class Subscription;
    friend class Msg;
    friend class ConnOptions;
    friend class SubOptions;
    template<typename T, PubAckHandler<T> callback> friend void
    PubAckHandlerCallback(const char * guid, const char * error, void * closure);
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(stanConnection * sc, stanSubscription * sub, const char * channel, stanMsg * msg, void * closure);
    template<typename T, ConnectionLostHandler<T> callback> friend void
    ConnectionLostHandlerCallback(stanConnection * sc, const char * errorTxt, void * closure);
    stanConnection * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit Connection(stanConnection* ptr) : self(ptr)
    {
    }

    Connection(Connection&& other) : self(other.Release())
    {
    }

    /** \brief Connects to a `NATS Streaming Server` using the provided options.
    *
    * @see #stanConnection_Connect()
    */
    Connection(const char * clusterID, const char * clientID, stanConnOptions * options);

    /** \brief Returns the underlying NATS Connection.
    *
    * @see #stanConnection_GetNATSConnection()
    */
    void
    GetNATSConnection(natsConnection ** nc);

    /** \brief Releases the NATS Connection.
    *
    * @see #stanConnection_ReleaseNATSConnection()
    */
    void
    ReleaseNATSConnection();

    /** \brief Closes the connection.
    *
    * @see #stanConnection_Close()
    */
    void
    Close();

    /** \brief Destroys the connection object.
    *
    * @see #stanConnection_Destroy()
    */
    ~Connection();

    /** \brief Publishes data on a channel.
    *
    * @see #stanConnection_Publish()
    */
    void
    Publish(const char * channel, const void * data, int dataLen);

    /** \brief Asynchronously publishes data on a channel.
    *
    * @see #stanConnection_PublishAsync()
    */
    void
    PublishAsync(const char * channel, const void * data, int dataLen, stanPubAckHandler ah, void * ahClosure);

    template<typename T1, PubAckHandler<T1> callback1> void
    PublishAsync(const char * channel, const void * data, int dataLen, T1 * ahClosure);

    /** \brief Creates a subscription.
    *
    * @see #stanConnection_Subscribe()
    */
    Subscription
    Subscribe(const char * channel, stanMsgHandler cb, void * cbClosure, stanSubOptions * options);

    template<typename T1, MsgHandler<T1> callback1> Subscription
    Subscribe(const char * channel, T1 * cbClosure, stanSubOptions * options);

    /** \brief Creates a queue subscription.
    *
    * @see #stanConnection_QueueSubscribe()
    */
    Subscription
    QueueSubscribe(const char * channel, const char * queueGroup, stanMsgHandler cb, void * cbClosure, stanSubOptions * options);

    template<typename T1, MsgHandler<T1> callback1> Subscription
    QueueSubscribe(const char * channel, const char * queueGroup, T1 * cbClosure, stanSubOptions * options);

    operator const stanConnection * () const
    {
        return self;
    }

    operator stanConnection * ()
    {
        return self;
    }

    [[nodiscard]] stanConnection *
    Release()
    {
        stanConnection * ret = self;
        self = nullptr;
        return ret;
    }
};

class Connection::WithoutDestuction : public Connection {
public:
    WithoutDestuction(stanConnection * ptr) : Connection(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

/** \brief Interest on a given channel.
 *
 * A #stanSubscription represents interest in a given channel.
 */
class Subscription {
    class WithoutDestuction;
    friend class Connection;
    friend class Msg;
    friend class ConnOptions;
    friend class SubOptions;
    template<typename T, PubAckHandler<T> callback> friend void
    PubAckHandlerCallback(const char * guid, const char * error, void * closure);
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(stanConnection * sc, stanSubscription * sub, const char * channel, stanMsg * msg, void * closure);
    template<typename T, ConnectionLostHandler<T> callback> friend void
    ConnectionLostHandlerCallback(stanConnection * sc, const char * errorTxt, void * closure);
    stanSubscription * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit Subscription(stanSubscription* ptr) : self(ptr)
    {
    }

    Subscription(Subscription&& other) : self(other.Release())
    {
    }

    /** \brief Sets a completion callback.
    *
    * @see #stanSubscription_SetOnCompleteCB()
    */
    void
    SetOnCompleteCB(natsOnCompleteCB cb, void * closure);

    template<typename T1, nats::OnCompleteCB<T1> callback1> void
    SetOnCompleteCB(T1 * closure);

    /** \brief Acknowledge a message.
    *
    * @see #stanSubscription_AckMsg()
    */
    void
    AckMsg(stanMsg * msg);

    /** \brief Permanently remove a subscription.
    *
    * @see #stanSubscription_Unsubscribe()
    */
    void
    Unsubscribe();

    /** \brief Closes the subscription.
    *
    * @see #stanSubscription_Close()
    */
    void
    Close();

    /** \brief Destroys the subscription.
    *
    * @see #stanSubscription_Destroy()
    */
    ~Subscription();

    operator const stanSubscription * () const
    {
        return self;
    }

    operator stanSubscription * ()
    {
        return self;
    }

    [[nodiscard]] stanSubscription *
    Release()
    {
        stanSubscription * ret = self;
        self = nullptr;
        return ret;
    }
};

class Subscription::WithoutDestuction : public Subscription {
public:
    WithoutDestuction(stanSubscription * ptr) : Subscription(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

/** \brief The Streaming message.
 *
 * #stanMsg is the object passed to the subscriptions' message callbacks.
 */
class Msg {
    class WithoutDestuction;
    friend class Connection;
    friend class Subscription;
    friend class ConnOptions;
    friend class SubOptions;
    template<typename T, PubAckHandler<T> callback> friend void
    PubAckHandlerCallback(const char * guid, const char * error, void * closure);
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(stanConnection * sc, stanSubscription * sub, const char * channel, stanMsg * msg, void * closure);
    template<typename T, ConnectionLostHandler<T> callback> friend void
    ConnectionLostHandlerCallback(stanConnection * sc, const char * errorTxt, void * closure);
    stanMsg * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit Msg(stanMsg* ptr) : self(ptr)
    {
    }

    Msg(Msg&& other) : self(other.Release())
    {
    }

    /** \brief Returns the message's sequence number.
    *
    * @see #stanMsg_GetSequence()
    */
    uint64_t
    GetSequence() const;

    /** \brief Returns the message's timestamp.
    *
    * @see #stanMsg_GetTimestamp()
    */
    int64_t
    GetTimestamp() const;

    /** \brief Returns the message's redelivered flag.
    *
    * @see #stanMsg_IsRedelivered()
    */
    bool
    IsRedelivered() const;

    /** \brief Returns the message payload.
    *
    * @see #stanMsg_GetData()
    */
    const char *
    GetData() const;

    /** \brief Returns the message length.
    *
    * @see #stanMsg_GetDataLength()
    */
    int
    GetDataLength() const;

    /** \brief Destroys the message object.
    *
    * @see #stanMsg_Destroy()
    */
    ~Msg();

    operator const stanMsg * () const
    {
        return self;
    }

    operator stanMsg * ()
    {
        return self;
    }

    [[nodiscard]] stanMsg *
    Release()
    {
        stanMsg * ret = self;
        self = nullptr;
        return ret;
    }
};

class Msg::WithoutDestuction : public Msg {
public:
    WithoutDestuction(stanMsg * ptr) : Msg(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

/** \brief Way to configure a #stanConnection.
 *
 * Options can be used to create a customized #stanConnection.
 */
class ConnOptions {
    class WithoutDestuction;
    friend class Connection;
    friend class Subscription;
    friend class Msg;
    friend class SubOptions;
    template<typename T, PubAckHandler<T> callback> friend void
    PubAckHandlerCallback(const char * guid, const char * error, void * closure);
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(stanConnection * sc, stanSubscription * sub, const char * channel, stanMsg * msg, void * closure);
    template<typename T, ConnectionLostHandler<T> callback> friend void
    ConnectionLostHandlerCallback(stanConnection * sc, const char * errorTxt, void * closure);
    stanConnOptions * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit ConnOptions(stanConnOptions* ptr) : self(ptr)
    {
    }

    ConnOptions(ConnOptions&& other) : self(other.Release())
    {
    }

    /** \brief Creates a #stanConnOptions object.
    *
    * @see #stanConnOptions_Create()
    */
    ConnOptions();

    /** \brief Sets the URL to connect to.
    *
    * @see #stanConnOptions_SetURL()
    */
    void
    SetURL(const char * url);

    /** \brief Sets the NATS Options to use to create the connection.
    *
    * @see #stanConnOptions_SetNATSOptions()
    */
    void
    SetNATSOptions(natsOptions * nOpts);

    /** \brief Sets the timeout for establishing a connection.
    *
    * @see #stanConnOptions_SetConnectionWait()
    */
    void
    SetConnectionWait(int64_t wait);

    /** \brief Sets the timeout for waiting for an ACK for a published message.
    *
    * @see #stanConnOptions_SetPubAckWait()
    */
    void
    SetPubAckWait(int64_t wait);

    /** \brief Sets the subject prefix the library sends the connect request to.
    *
    * @see #stanConnOptions_SetDiscoveryPrefix()
    */
    void
    SetDiscoveryPrefix(const char * prefix);

    /** \brief Sets the maximum number of published messages without outstanding ACKs from the server.
    *
    * @see #stanConnOptions_SetMaxPubAcksInflight()
    */
    void
    SetMaxPubAcksInflight(int maxPubAcksInflight, float percentage);

    /** \brief Sets the ping interval and max out values.
    *
    * @see #stanConnOptions_SetPings()
    */
    void
    SetPings(int interval, int maxOut);

    /** \brief Sets the connection lost handler.
    *
    * @see #stanConnOptions_SetConnectionLostHandler()
    */
    void
    SetConnectionLostHandler(stanConnectionLostHandler handler, void * closure);

    template<typename T1, ConnectionLostHandler<T1> callback1> void
    SetConnectionLostHandler(T1 * closure);

    /** \brief Destroys a #stanConnOptions object.
    *
    * @see #stanConnOptions_Destroy()
    */
    ~ConnOptions();

    operator const stanConnOptions * () const
    {
        return self;
    }

    operator stanConnOptions * ()
    {
        return self;
    }

    [[nodiscard]] stanConnOptions *
    Release()
    {
        stanConnOptions * ret = self;
        self = nullptr;
        return ret;
    }
};

class ConnOptions::WithoutDestuction : public ConnOptions {
public:
    WithoutDestuction(stanConnOptions * ptr) : ConnOptions(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

/** \brief Way to configure a #stanSubscription.
 *
 * Options can be used to create a customized #stanSubscription.
 */
class SubOptions {
    class WithoutDestuction;
    friend class Connection;
    friend class Subscription;
    friend class Msg;
    friend class ConnOptions;
    template<typename T, PubAckHandler<T> callback> friend void
    PubAckHandlerCallback(const char * guid, const char * error, void * closure);
    template<typename T, MsgHandler<T> callback> friend void
    MsgHandlerCallback(stanConnection * sc, stanSubscription * sub, const char * channel, stanMsg * msg, void * closure);
    template<typename T, ConnectionLostHandler<T> callback> friend void
    ConnectionLostHandlerCallback(stanConnection * sc, const char * errorTxt, void * closure);
    stanSubOptions * self;

    void
    disableDestroy()
    {
        self = nullptr;
    }

public:
    explicit SubOptions(stanSubOptions* ptr) : self(ptr)
    {
    }

    SubOptions(SubOptions&& other) : self(other.Release())
    {
    }

    /** \brief Creates a #stanSubOptions object.
    *
    * @see #stanSubOptions_Create()
    */
    SubOptions();

    /** \brief Sets the Durable Name for this subscription.
    *
    * @see #stanSubOptions_SetDurableName()
    */
    void
    SetDurableName(const char * durableName);

    /** \brief Sets the timeout for waiting for an ACK from the cluster's point of view for delivered messages.
    *
    * @see #stanSubOptions_SetAckWait()
    */
    void
    SetAckWait(int64_t wait);

    /** \brief Sets the the maximum number of messages the cluster will send without an ACK.
    *
    * @see #stanSubOptions_SetMaxInflight()
    */
    void
    SetMaxInflight(int maxInflight);

    /** \brief Sets the desired start position based on the given sequence number.
    *
    * @see #stanSubOptions_StartAtSequence()
    */
    void
    StartAtSequence(uint64_t seq);

    /** \brief Sets the desired start position based on the given time.
    *
    * @see #stanSubOptions_StartAtTime()
    */
    void
    StartAtTime(int64_t time);

    /** \brief Sets the desired start position based on the given delta.
    *
    * @see #stanSubOptions_StartAtTimeDelta()
    */
    void
    StartAtTimeDelta(int64_t delta);

    /** \brief The subscription should start with the last message in the channel.
    *
    * @see #stanSubOptions_StartWithLastReceived()
    */
    void
    StartWithLastReceived();

    /** \brief The subscription should start with the first message in the channel.
    *
    * @see #stanSubOptions_DeliverAllAvailable()
    */
    void
    DeliverAllAvailable();

    /** \brief Sets the subscription's acknowledgment mode.
    *
    * @see #stanSubOptions_SetManualAckMode()
    */
    void
    SetManualAckMode(bool manual);

    /** \brief Destroys a #stanSubOptions object.
    *
    * @see #stanSubOptions_Destroy()
    */
    ~SubOptions();

    operator const stanSubOptions * () const
    {
        return self;
    }

    operator stanSubOptions * ()
    {
        return self;
    }

    [[nodiscard]] stanSubOptions *
    Release()
    {
        stanSubOptions * ret = self;
        self = nullptr;
        return ret;
    }
};

class SubOptions::WithoutDestuction : public SubOptions {
public:
    WithoutDestuction(stanSubOptions * ptr) : SubOptions(ptr)
    {
    }

    ~WithoutDestuction()
    {
        disableDestroy();
    }
};

inline Connection::Connection(const char * clusterID, const char * clientID, stanConnOptions * options)
{
    Exception::CheckResult(stanConnection_Connect(&self, clusterID, clientID, options));
}

inline void
Connection::GetNATSConnection(natsConnection ** nc)
{
    return Exception::CheckResult(stanConnection_GetNATSConnection(self, nc));
}

inline void
Connection::ReleaseNATSConnection()
{
    return stanConnection_ReleaseNATSConnection(self);
}

inline void
Connection::Close()
{
    return Exception::CheckResult(stanConnection_Close(self));
}

inline Connection::~Connection()
{
    Exception::CheckResult(stanConnection_Destroy(self));
}

inline void
Connection::Publish(const char * channel, const void * data, int dataLen)
{
    return Exception::CheckResult(stanConnection_Publish(self, channel, data, dataLen));
}

inline void
Connection::PublishAsync(const char * channel, const void * data, int dataLen, stanPubAckHandler ah, void * ahClosure)
{
    return Exception::CheckResult(stanConnection_PublishAsync(self, channel, data, dataLen, ah, ahClosure));
}

template<typename T1, PubAckHandler<T1> callback1> inline void
Connection::PublishAsync(const char * channel, const void * data, int dataLen, T1 * ahClosure)
{
    return PublishAsync(channel, data, dataLen, &PubAckHandlerCallback<T1, callback1>, ahClosure);
}

inline Subscription
Connection::Subscribe(const char * channel, stanMsgHandler cb, void * cbClosure, stanSubOptions * options)
{
    Subscription ret(nullptr);
    Exception::CheckResult(stanConnection_Subscribe(&ret.self, self, channel, cb, cbClosure, options));
    return ret;
}

template<typename T1, MsgHandler<T1> callback1> inline Subscription
Connection::Subscribe(const char * channel, T1 * cbClosure, stanSubOptions * options)
{
    return Subscribe(channel, &MsgHandlerCallback<T1, callback1>, cbClosure, options);
}

inline Subscription
Connection::QueueSubscribe(const char * channel, const char * queueGroup, stanMsgHandler cb, void * cbClosure, stanSubOptions * options)
{
    Subscription ret(nullptr);
    Exception::CheckResult(stanConnection_QueueSubscribe(&ret.self, self, channel, queueGroup, cb, cbClosure, options));
    return ret;
}

template<typename T1, MsgHandler<T1> callback1> inline Subscription
Connection::QueueSubscribe(const char * channel, const char * queueGroup, T1 * cbClosure, stanSubOptions * options)
{
    return QueueSubscribe(channel, queueGroup, &MsgHandlerCallback<T1, callback1>, cbClosure, options);
}

inline void
Subscription::SetOnCompleteCB(natsOnCompleteCB cb, void * closure)
{
    return Exception::CheckResult(stanSubscription_SetOnCompleteCB(self, cb, closure));
}

template<typename T1, nats::OnCompleteCB<T1> callback1> inline void
Subscription::SetOnCompleteCB(T1 * closure)
{
    return SetOnCompleteCB(&nats::OnCompleteCBCallback<T1, callback1>, closure);
}

inline void
Subscription::AckMsg(stanMsg * msg)
{
    return Exception::CheckResult(stanSubscription_AckMsg(self, msg));
}

inline void
Subscription::Unsubscribe()
{
    return Exception::CheckResult(stanSubscription_Unsubscribe(self));
}

inline void
Subscription::Close()
{
    return Exception::CheckResult(stanSubscription_Close(self));
}

inline Subscription::~Subscription()
{
    stanSubscription_Destroy(self);
}

inline uint64_t
Msg::GetSequence() const
{
    return stanMsg_GetSequence(self);
}

inline int64_t
Msg::GetTimestamp() const
{
    return stanMsg_GetTimestamp(self);
}

inline bool
Msg::IsRedelivered() const
{
    return stanMsg_IsRedelivered(self);
}

inline const char *
Msg::GetData() const
{
    return stanMsg_GetData(self);
}

inline int
Msg::GetDataLength() const
{
    return stanMsg_GetDataLength(self);
}

inline Msg::~Msg()
{
    stanMsg_Destroy(self);
}

inline ConnOptions::ConnOptions()
{
    Exception::CheckResult(stanConnOptions_Create(&self));
}

inline void
ConnOptions::SetURL(const char * url)
{
    return Exception::CheckResult(stanConnOptions_SetURL(self, url));
}

inline void
ConnOptions::SetNATSOptions(natsOptions * nOpts)
{
    return Exception::CheckResult(stanConnOptions_SetNATSOptions(self, nOpts));
}

inline void
ConnOptions::SetConnectionWait(int64_t wait)
{
    return Exception::CheckResult(stanConnOptions_SetConnectionWait(self, wait));
}

inline void
ConnOptions::SetPubAckWait(int64_t wait)
{
    return Exception::CheckResult(stanConnOptions_SetPubAckWait(self, wait));
}

inline void
ConnOptions::SetDiscoveryPrefix(const char * prefix)
{
    return Exception::CheckResult(stanConnOptions_SetDiscoveryPrefix(self, prefix));
}

inline void
ConnOptions::SetMaxPubAcksInflight(int maxPubAcksInflight, float percentage)
{
    return Exception::CheckResult(stanConnOptions_SetMaxPubAcksInflight(self, maxPubAcksInflight, percentage));
}

inline void
ConnOptions::SetPings(int interval, int maxOut)
{
    return Exception::CheckResult(stanConnOptions_SetPings(self, interval, maxOut));
}

inline void
ConnOptions::SetConnectionLostHandler(stanConnectionLostHandler handler, void * closure)
{
    return Exception::CheckResult(stanConnOptions_SetConnectionLostHandler(self, handler, closure));
}

template<typename T1, ConnectionLostHandler<T1> callback1> inline void
ConnOptions::SetConnectionLostHandler(T1 * closure)
{
    return SetConnectionLostHandler(&ConnectionLostHandlerCallback<T1, callback1>, closure);
}

inline ConnOptions::~ConnOptions()
{
    stanConnOptions_Destroy(self);
}

inline SubOptions::SubOptions()
{
    Exception::CheckResult(stanSubOptions_Create(&self));
}

inline void
SubOptions::SetDurableName(const char * durableName)
{
    return Exception::CheckResult(stanSubOptions_SetDurableName(self, durableName));
}

inline void
SubOptions::SetAckWait(int64_t wait)
{
    return Exception::CheckResult(stanSubOptions_SetAckWait(self, wait));
}

inline void
SubOptions::SetMaxInflight(int maxInflight)
{
    return Exception::CheckResult(stanSubOptions_SetMaxInflight(self, maxInflight));
}

inline void
SubOptions::StartAtSequence(uint64_t seq)
{
    return Exception::CheckResult(stanSubOptions_StartAtSequence(self, seq));
}

inline void
SubOptions::StartAtTime(int64_t time)
{
    return Exception::CheckResult(stanSubOptions_StartAtTime(self, time));
}

inline void
SubOptions::StartAtTimeDelta(int64_t delta)
{
    return Exception::CheckResult(stanSubOptions_StartAtTimeDelta(self, delta));
}

inline void
SubOptions::StartWithLastReceived()
{
    return Exception::CheckResult(stanSubOptions_StartWithLastReceived(self));
}

inline void
SubOptions::DeliverAllAvailable()
{
    return Exception::CheckResult(stanSubOptions_DeliverAllAvailable(self));
}

inline void
SubOptions::SetManualAckMode(bool manual)
{
    return Exception::CheckResult(stanSubOptions_SetManualAckMode(self, manual));
}

inline SubOptions::~SubOptions()
{
    stanSubOptions_Destroy(self);
}

template<typename T, PubAckHandler<T> callback> void
PubAckHandlerCallback(const char * guid, const char * error, void * closure)
{
    T * self = static_cast<T *>(closure);
    return (self->*callback)(guid, error);
}

template<typename T, MsgHandler<T> callback> void
MsgHandlerCallback(stanConnection * sc, stanSubscription * sub, const char * channel, stanMsg * msg, void * closure)
{
    Connection::WithoutDestuction sc_(sc);
    Subscription::WithoutDestuction sub_(sub);
    Msg::WithoutDestuction msg_(msg);
    T * self = static_cast<T *>(closure);
    return (self->*callback)(sc_, sub_, channel, msg_);
}

template<typename T, ConnectionLostHandler<T> callback> void
ConnectionLostHandlerCallback(stanConnection * sc, const char * errorTxt, void * closure)
{
    Connection::WithoutDestuction sc_(sc);
    T * self = static_cast<T *>(closure);
    return (self->*callback)(sc_, errorTxt);
}

} // namespace stan
#endif


#endif /* NATS_HPP_ */
