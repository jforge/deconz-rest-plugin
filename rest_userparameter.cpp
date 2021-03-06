/*
 * Copyright (c) 2016 dresden elektronik ingenieurtechnik gmbh.
 * All rights reserved.
 *
 * The software in this package is published under the terms of the BSD
 * style license a copy of which has been included with this distribution in
 * the LICENSE.txt file.
 *
 */

#include <QApplication>
#include <QDesktopServices>
#include <QString>
#include <QTcpSocket>
#include <QVariantMap>
#include <QNetworkInterface>
#include "de_web_plugin.h"
#include "de_web_plugin_private.h"
#include "json.h"
#include <stdlib.h>

/*! Configuration REST API broker.
    \param req - request data
    \param rsp - response data
    \return REQ_READY_SEND
            REQ_NOT_HANDLED
 */
int DeRestPluginPrivate::handleUserparameterApi(const ApiRequest &req, ApiResponse &rsp)
{
    // POST /api/<apikey>/userparameter/<parameter>
    if ((req.path.size() == 4) && (req.hdr.method() == "POST") && (req.path[2] == "userparameter"))
    {
        return addUserParameter(req, rsp);
    }
    // PUT /api/<apikey>/userparameter/<parameter>
    else if ((req.path.size() == 4) && (req.hdr.method() == "PUT") && (req.path[2] == "userparameter"))
    {
        return modifyUserParameter(req, rsp);
    }
    // GET /api/<apikey>/userparameter
    else if ((req.path.size() == 3) && (req.hdr.method() == "GET") && (req.path[2] == "userparameter"))
    {
        return getAllUserParameter(req, rsp);
    }
    // GET /api/<apikey>/userparameter/<parameter>
    else if ((req.path.size() == 4) && (req.hdr.method() == "GET") && (req.path[2] == "userparameter"))
    {
        return getUserParameter(req, rsp);
    }
    // DELETE /api/<apikey>/userparameter/<parameter>
    else if ((req.path.size() == 4) && (req.hdr.method() == "DELETE") && (req.path[2] == "userparameter"))
    {
        return deleteUserParameter(req, rsp);
    }
    return REQ_NOT_HANDLED;
}

/*! POST /api/<apikey>/userparameter/<parameter>
    \return REQ_READY_SEND
            REQ_NOT_HANDLED
 */
int DeRestPluginPrivate::addUserParameter(const ApiRequest &req, ApiResponse &rsp)
{
    if(!checkApikeyAuthentification(req, rsp))
    {
        return REQ_READY_SEND;
    }

    DBG_Assert(req.path.size() == 4);

    if (req.path.size() != 4)
    {
        return -1;
    }

    const QString &key = req.path[3];

    rsp.httpStatus = HttpStatusOk;

    //don't overwrite existing parameters if POST request
    if (gwUserParameter.contains(key))
    {
        rsp.httpStatus = HttpStatusBadRequest;
        rsp.list.append(errorToMap(ERR_DUPLICATE_EXIST ,
        QString("config/userparameter"), QString("key %1 already exists").arg(key)));
        return REQ_READY_SEND;
    }

    QVariantMap rspItem;
    QVariantMap rspItemState;

    gwUserParameter.insert(key, req.content);
    rspItemState["/config/userparameter"] = QString("added new %1").arg(key);
    rspItem["success"] = rspItemState;
    rsp.list.append(rspItem);

    queSaveDb(DB_USERPARAM, DB_SHORT_SAVE_DELAY);
    return REQ_READY_SEND;
}

/*! PUT /api/<apikey>/userparameter/<parameter>
    \return REQ_READY_SEND
            REQ_NOT_HANDLED
 */
int DeRestPluginPrivate::modifyUserParameter(const ApiRequest &req, ApiResponse &rsp)
{
    if(!checkApikeyAuthentification(req, rsp))
    {
        return REQ_READY_SEND;
    }

    DBG_Assert(req.path.size() == 4);

    if (req.path.size() != 4)
    {
        return -1;
    }

    const QString &key = req.path[3];

    rsp.httpStatus = HttpStatusOk;

    QVariantMap rspItem;
    QVariantMap rspItemState;

    //overwrite existing parameters if PUT request
    if (gwUserParameter.contains(key))
    {
        QVariantMap::iterator it = gwUserParameter.find(key);
        gwUserParameter.erase(it);
        gwUserParameter.insert(key, req.content);
        rspItemState["/config/userparameter"] = QString("updated %1").arg(key);
        rspItem["success"] = rspItemState;
        rsp.list.append(rspItem);
    }
    else
    {
        gwUserParameter.insert(key, req.content);
        rspItemState["/config/userparameter"] = QString("added new %1").arg(key);
        rspItem["success"] = rspItemState;
        rsp.list.append(rspItem);
    }

    queSaveDb(DB_USERPARAM, DB_SHORT_SAVE_DELAY);
    return REQ_READY_SEND;
}

/*! GET /api/<apikey>/userparameter
    \return REQ_READY_SEND
            REQ_NOT_HANDLED
 */
int DeRestPluginPrivate::getAllUserParameter(const ApiRequest &req, ApiResponse &rsp)
{
    Q_UNUSED(req);

    if(!checkApikeyAuthentification(req, rsp))
    {
        return REQ_READY_SEND;
    }

    rsp.httpStatus = HttpStatusOk;

    QVariantMap::const_iterator k = gwUserParameter.begin();
    QVariantMap::const_iterator k_end = gwUserParameter.end();

    int i = 0;
    for (; k != k_end; ++k)
    {
        rsp.map[QString("key %1").arg(i)] = k.key();
        i++;
    }

    if (rsp.map.isEmpty())
    {
        rsp.str = "{}"; // return empty object
    }

    return REQ_READY_SEND;
}

/*! GET /api/<apikey>/userparameter/<parameter>
    \return REQ_READY_SEND
            REQ_NOT_HANDLED
 */
int DeRestPluginPrivate::getUserParameter(const ApiRequest &req, ApiResponse &rsp)
{
    Q_UNUSED(req);

    if(!checkApikeyAuthentification(req, rsp))
    {
        return REQ_READY_SEND;
    }

    DBG_Assert(req.path.size() == 4);

    if (req.path.size() != 4)
    {
        return -1;
    }

    const QString &key = req.path[3];

    rsp.httpStatus = HttpStatusOk;

    if (gwUserParameter.contains(key))
    {
        rsp.map[key] =  gwUserParameter.value(key);
        queSaveDb(DB_USERPARAM, DB_SHORT_SAVE_DELAY);
    }
    else
    {
        QVariantMap rspItem;
        QVariantMap rspItemState;
        rspItemState["/config/userparameter"] = QString("key %1 not found").arg(key);
        rspItem["error"] = rspItemState;
        rsp.list.append(rspItem);
        rsp.httpStatus = HttpStatusNotFound;
    }

    return REQ_READY_SEND;
}

/*! DELETE /api/<apikey>/userparameter/<parameter>
    \return REQ_READY_SEND
            REQ_NOT_HANDLED
 */
int DeRestPluginPrivate::deleteUserParameter(const ApiRequest &req, ApiResponse &rsp)
{
    if(!checkApikeyAuthentification(req, rsp))
    {
        return REQ_READY_SEND;
    }

    DBG_Assert(req.path.size() == 4);

    if (req.path.size() != 4)
    {
        return -1;
    }

    const QString &key = req.path[3];

    QVariantMap rspItem;
    QVariantMap rspItemState;

    if (gwUserParameter.contains(key))
    {
        gwUserParameter.remove(key);
        rspItemState["/config/userparameter"] = QString("key %1 removed").arg(key);
        rspItem["success"] = rspItemState;
        rsp.list.append(rspItem);
        rsp.httpStatus = HttpStatusOk;
        queSaveDb(DB_USERPARAM, DB_SHORT_SAVE_DELAY);
    }
    else
    {
        rspItemState["/config/userparameter"] = QString("key %1 not found").arg(key);
        rspItem["error"] = rspItemState;
        rsp.list.append(rspItem);
        rsp.httpStatus = HttpStatusNotFound;
    }

    return REQ_READY_SEND;
}
