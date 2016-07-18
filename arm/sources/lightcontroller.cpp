#include "lightcontroller.hpp"
#include <QtWebSockets/qwebsocketserver.h>
#include <QtWebSockets/qwebsocket.h>

#include <QtCore/QDebug>

QT_USE_NAMESPACE

LightController::LightController(quint16 port, bool debug, QObject *parent) :
	QObject(parent),
	m_pWebSocketServer(new QWebSocketServer(
						   QStringLiteral("Light Controller"),
						   QWebSocketServer::NonSecureMode, this)),
	m_clients(),
	m_debug(debug)
{
	if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
		if (m_debug) qDebug() << "LightController listening on port" << port;

		connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
				this, &LightController::onNewConnection);
		connect(m_pWebSocketServer, &QWebSocketServer::closed, 
				this, &LightController::closed);
	}
}

LightController::~LightController()
{
	m_pWebSocketServer->close();
	qDeleteAll(m_clients.begin(), m_clients.end());
}

void LightController::onNewConnection()
{
	QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

	connect(pSocket, &QWebSocket::textMessageReceived,
			this, &LightController::processTextMessage);
	connect(pSocket, &QWebSocket::binaryMessageReceived,
			this, &LightController::processBinaryMessage);
	connect(pSocket, &QWebSocket::disconnected,
			this, &LightController::socketDisconnected);

	m_clients << pSocket;
}

void LightController::processTextMessage(QString message)
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (m_debug) qDebug() << "Message received:" << message;
	if (pClient) {
		pClient->sendTextMessage(message);
	}
}

void LightController::processBinaryMessage(QByteArray message)
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (m_debug) qDebug() << "Binary Message received:" << message;
	if (pClient) {
		pClient->sendBinaryMessage(message);
	}
}

void LightController::socketDisconnected()
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (m_debug) qDebug() << "socketDisconnected:" << pClient;
	if (pClient) {
		m_clients.removeAll(pClient);
		pClient->deleteLater();
	}
}

