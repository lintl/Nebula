/*******************************************************************************
 * Project:  Nebula
 * @file     Channel.hpp
 * @brief    通信通道
 * @author   Bwar
 * @date:    2016年8月10日
 * @note     通信通道，如Socket连接
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CHANNEL_CHANNEL_HPP_
#define SRC_CHANNEL_CHANNEL_HPP_

#include "ev.h"
#include "util/CBuffer.hpp"
#include "util/StreamCodec.hpp"
#include "util/json/CJsonObject.hpp"

#include "codec/Codec.hpp"
#include "Definition.hpp"

namespace neb
{

//typedef void (*IoCallbackFun)(struct ev_loop *, struct ev_io*, int);
//typedef void (*TimerCallbackFun)(struct ev_loop *, ev_timer*, int);


class Labor;
class Worker;
class Manager;

class Channel
{
public:
    Channel(int iFd, uint32 ulSeq);
    virtual ~Channel();

    // bool Init(E_CODEC_TYPE eCodecType, IoCallbackFun io_cb, TimerCallbackFun timer_cb, const std::string& strKey = "That's a lizard.");
    bool Init(E_CODEC_TYPE eCodecType, const std::string& strKey = "That's a lizard.");

    E_CODEC_TYPE GetCodecType() const;

    E_CODEC_STATUS Send();
    E_CODEC_STATUS Send(uint32 uiCmd, uint32 uiSeq, const MsgBody& oMsgBody);
    E_CODEC_STATUS Send(const HttpMsg& oHttpMsg, uint32 ulStepSeq);
    E_CODEC_STATUS Recv(MsgHead& oMsgHead, MsgBody& oMsgBody);
    E_CODEC_STATUS Recv(HttpMsg& oHttpMsg);
    E_CODEC_STATUS Recv(MsgHead& oMsgHead, MsgBody& oMsgBody, HttpMsg& oHttpMsg);
    E_CODEC_STATUS Fetch(MsgHead& oMsgHead, MsgBody& oMsgBody);
    E_CODEC_STATUS Fetch(HttpMsg& oHttpMsg);
    E_CODEC_STATUS Fetch(MsgHead& oMsgHead, MsgBody& oMsgBody, HttpMsg& oHttpMsg);

public:
    int GetFd() const
    {
        return(m_iFd);
    }

    uint32 GetSequence() const
    {
        return(m_ulSeq);
    }

    uint32 GetStepSeq() const
    {
        return(m_ulStepSeq);
    }

    ev_tstamp GetActiveTime() const
    {
        return(m_dActiveTime);
    }

    ev_tstamp GetKeepAlive();

    uint8 GetChannelStatus() const
    {
        return(m_ucChannelStatus);
    }

    const std::string& GetIdentify() const
    {
        return(m_strIdentify);
    }

    const std::string& GetRemoteAddr() const
    {
        return(m_strRemoteAddr);
    }

    const std::string& GetClientData() const
    {
        return(m_strClientData);
    }

    bool IsChannelVerify() const
    {
        return(m_strClientData.size());
    }

    bool NeedAliveCheck() const;

    uint32 GetMsgNum() const
    {
        return(m_ulMsgNum);
    }

    uint32 GetUnitTimeMsgNum() const
    {
        return(m_ulUnitTimeMsgNum);
    }

    int GetErrno() const
    {
        return(m_iErrno);
    }

    const std::string& GetErrMsg() const
    {
        return(m_strErrMsg);
    }

protected:
    log4cplus::Logger& GetLogger()
    {
        return (*m_pLogger);
    }

    log4cplus::Logger* GetLoggerPtr()
    {
        return (m_pLogger);
    }

private:
    void SetLogger(log4cplus::Logger* pLogger)
    {
        m_pLogger = pLogger;
    }

    void SetLabor(Labor* pLabor)
    {
        m_pLabor = pLabor;
    }

    /*
    void SetActiveTime(ev_tstamp dTime)
    {
        m_dActiveTime = dTime;
    }
    */

    void SetKeepAlive(ev_tstamp dTime)
    {
        m_dKeepAlive = dTime;
    }

    void SetChannelStatus(E_CHANNEL_STATUS eStatus)
    {
        m_ucChannelStatus = (uint8)eStatus;
    }

    void SetChannelStatus(E_CHANNEL_STATUS eStatus, uint32 ulStepSeq)
    {
        m_ucChannelStatus = (uint8)eStatus;
        m_ulStepSeq = ulStepSeq;
    }

    void SetClientData(const std::string& strClientData)
    {
        m_strClientData = strClientData;
    }

    void SetIdentify(const std::string& strIdentify)
    {
        m_strIdentify = strIdentify;
    }

    void SetRemoteAddr(const std::string& strRemoteAddr)
    {
        m_strRemoteAddr = strRemoteAddr;
    }

    bool SwitchCodec(E_CODEC_TYPE eCodecType, ev_tstamp dKeepAlive);

    ev_io* AddIoWatcher();
    ev_io* MutableIoWatcher()
    {
        return(m_pIoWatcher);
    }

    ev_timer* AddTimerWatcher();
    ev_timer* MutableTimerWatcher()
    {
        return(m_pTimerWatcher);
    }

private:
    uint8 m_ucChannelStatus;
    char m_szErrBuff[256];
    int32 m_iFd;                          ///< 文件描述符
    uint32 m_ulSeq;                       ///< 文件描述符创建时对应的序列号
    uint32 m_ulForeignSeq;                ///< 外来的seq，每个连接的包都是有序的，用作接入Server数据包检查，防止篡包
    uint32 m_ulStepSeq;                   ///< 正在等待回调的Step seq（比如发出一个HttpPost或HttpGet请求，m_ulStepSeq即为正在等待响应的HttpStep的seq）
    uint32 m_ulUnitTimeMsgNum;            ///< 统计单位时间内接收消息数量
    uint32 m_ulMsgNum;                    ///< 接收消息数量
    ev_tstamp m_dActiveTime;              ///< 最后一次访问时间
    ev_tstamp m_dKeepAlive;               ///< 连接保持时间，默认值0为用心跳保持的长连接，大于0的值不做心跳检查，时间到即断连接,小于0为收完数据立即断开连接（主要用于http连接）
    ev_io* m_pIoWatcher;                  ///< 不在结构体析构时回收
    ev_timer* m_pTimerWatcher;            ///< 不在结构体析构时回收
    CBuffer* m_pRecvBuff;
    CBuffer* m_pSendBuff;
    CBuffer* m_pWaitForSendBuff;    ///< 等待发送的数据缓冲区（数据到达时，连接并未建立，等连接建立并且pSendBuff发送完毕后立即发送）
    Codec* m_pCodec;                      ///< 编解码器
    int m_iErrno;
    std::string m_strKey;                 ///< 密钥
    std::string m_strClientData;         ///< 客户端相关数据（例如IM里的用户昵称、头像等，登录或连接时保存起来，后续发消息或其他操作无须客户端再带上来）
    std::string m_strErrMsg;
    std::string m_strIdentify;            ///< 连接标识（可以为空，不为空时用于标识业务层与连接的关系）
    std::string m_strRemoteAddr;          ///< 对端IP地址（不是客户端地址，但可能跟客户端地址相同）
    log4cplus::Logger* m_pLogger;
    Labor* m_pLabor;

    friend class Worker;
    friend class Manager;
};

} /* namespace neb */

#endif /* SRC_CHANNEL_CHANNEL_HPP_ */
