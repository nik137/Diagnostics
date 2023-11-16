package com.ap_impulse.websocket;

import java.io.IOException;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArraySet;

import org.springframework.stereotype.Component;

import jakarta.websocket.OnClose;
import jakarta.websocket.OnError;
import jakarta.websocket.OnMessage;
import jakarta.websocket.OnOpen;
import jakarta.websocket.Session;
import jakarta.websocket.server.ServerEndpoint;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@Component
@ServerEndpoint(value = "/")
public class Socket {
    private  Session session;
    public static Set<Socket> listeners = new CopyOnWriteArraySet<>();

    @OnOpen
    public void onOpen(Session session) throws InterruptedException {
        this.session = session;
        listeners.add(this);
        log.info(String.format("New session connected! Connected listeners: %s", listeners.size()));
        System.out.println("Open");
    }

    @OnMessage 
    public void onMessage(String message) {
        broadcast(message);
        System.out.println("omMessage");
    }

    @OnClose
    public void onClose(Session session) {
        listeners.remove(this);
        log.info(String.format("Session disconnected. Total connected listeners: %s", listeners.size()));
    }

    @OnError
    public void onError(Session session, Throwable throwable) {
        //Error
    }

    public static void broadcast(String message) {
        for (Socket listener : listeners) {
            listener.sendMessage(message);
        }
    }

    private void sendMessage(String message) {
        try {
            session.getBasicRemote().sendText(message);
        } catch (IOException e) {
            log.error("Caught exception while sending message to Session Id: " + session.getId(), e.getMessage(), e);
        }
    }
}

