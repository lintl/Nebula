/*******************************************************************************
 * Project:  Nebula
 * @file     RedisStep.cpp
 * @brief 
 * @author   Bwar
 * @date:    2016年8月30日
 * @note
 * Modify history:
 ******************************************************************************/
#include "RedisStep.hpp"

namespace neb
{

RedisStep::RedisStep(Step* pNextStep)
    : Step(OBJ_REDIS_STEP, pNextStep), m_pRedisCmd(NULL)
{
    m_pRedisCmd = new RedisCmd();
}

RedisStep::RedisStep(const tagChannelContext& stCtx, const MsgHead& oReqMsgHead, const MsgBody& oReqMsgBody, Step* pNextStep)
    : Step(OBJ_REDIS_STEP, pNextStep),
      m_stCtx(stCtx), m_oReqMsgHead(oReqMsgHead), m_oReqMsgBody(oReqMsgBody),
      m_pRedisCmd(NULL)
{
    m_pRedisCmd = new RedisCmd();
}

RedisStep::~RedisStep()
{
    if (m_pRedisCmd != NULL)
    {
        delete m_pRedisCmd;
        m_pRedisCmd = NULL;
    }
}

} /* namespace neb */
