/*
 * Copyright (c) 2016 dresden elektronik ingenieurtechnik gmbh.
 * All rights reserved.
 *
 * The software in this package is published under the terms of the BSD
 * style license a copy of which has been included with this distribution in
 * the LICENSE.txt file.
 *
 */

#ifndef RULE_H
#define RULE_H

#include <stdint.h>
#include <QString>
#include <vector>
#include <QDateTime>
#include <deconz.h>
#include "bindings.h"
#include "json.h"

class RuleCondition;
class RuleAction;
class RestNodeBase;

/*! Helper class to handle ZigBee binding/unbinding for Rules. */
class BindingTask
{
public:
    enum Constants
    {
        Timeout = 20,
        Retries = 2
    };

    enum State
    {
        StateIdle,
        StateInProgress,
        StateCheck,
        StateFinished
    };

    enum Action
    {
        ActionBind,
        ActionUnbind
    };

    BindingTask():
        action(ActionBind),
        state(StateCheck),
        timeout(BindingTask::Timeout),
        retries(BindingTask::Retries),
        restNode(0)
    {
    }

    bool operator==(const BindingTask &rhs) const;
    bool operator!=(const BindingTask &rhs) const;

    Action action;
    State state;

    quint8 zdpSeqNum;
    int timeout; // seconds
    int retries;
    RestNodeBase *restNode;

    Binding binding;
};

/*! \class Rule

    Represents a Rest API Rule.
 */
class Rule
{
public:
    Rule();

    enum State
    {
        StateNormal,
        StateDeleted
    };

    enum Constants
    {
        MaxVerifyDelay = 300
    };

    State state() const;
    void setState(State state);
    const QString &id() const;
    void setId(const QString &id);
    const QString &name() const;
    void setName(const QString &name);
    const QString &lastTriggered() const;
    void setLastTriggered(const QString &lastTriggered);
    const QTime &lastTriggeredTime() const;
    const QString &creationtime() const;
    void setCreationtime(const QString &creationtime);
    const quint32 &timesTriggered() const;
    void setTimesTriggered(const quint32 &timesTriggered);
    int triggerPeriodic() const;
    void setTriggerPeriodic(int ms);
    const QString &owner() const;
    void setOwner(const QString &owner);
    const QString &status() const;
    void setStatus(const QString &status);
    const std::vector<RuleCondition> &conditions() const;
    void setConditions(const std::vector<RuleCondition> &conditions);
    const std::vector<RuleAction> &actions() const;
    void setActions(const std::vector<RuleAction> &actions);

    static QString actionsToString(const std::vector<RuleAction> &actions);
    static QString conditionsToString(const std::vector<RuleCondition> &conditions);

    static std::vector<RuleAction> jsonToActions(const QString &json);
    static std::vector<RuleCondition> jsonToConditions(const QString &json);

    QString etag;
    int lastVerify; // copy of idleTotalCounter at last verification

private:
    State m_state;
    QString m_id;
    QString m_name;
    QTime m_lastTriggeredTime;
    QString m_lastTriggered;
    QString m_creationtime;
    quint32 m_timesTriggered;
    int m_triggerPeriodic;
    QString m_owner;
    QString m_status;
    std::vector<RuleCondition> m_conditions;
    std::vector<RuleAction> m_actions;
};

class RuleAction
{
public:
    RuleAction();

    const QString &address() const;
    void setAddress(const QString &address);
    const QString &method() const;
    void setMethod(const QString &method);
    const QString &body() const;
    void setBody(const QString &body);
    bool operator==(const RuleAction &other) const;

private:
    QString m_address;
    QString m_method;
    QString m_body;
};


class RuleCondition
{
public:
    RuleCondition();

    const QString &address() const;
    void setAddress(const QString &address);
    const QString &ooperator() const;
    void setOperator(const QString &ooperator);
    const QString &value() const;
    void setValue(const QString &value);
    bool operator==(const RuleCondition &other) const;

private:
    QString m_address;
    QString m_operator;
    QString m_value;
};
#endif // RULE_H
