package com.ap_impulse.websocket;
/**
* Answer.java Повернення відповіді клієнту (браузеру), для протоколу STOMP, WebSocket. 
* <p>
* Простий об'єкт для повернення відповіді клієнту (браузеру), для протоколу STOMP, WebSocket.  
* <pre>
* {
     "name": "text",
     "text": "text"
     "time": "time"
  }
* </pre>
* @param  name, text, time
* @return name, text, time
*/ 
public class Answer {

	private String name; 
	private String text; 
	private String time; 


	public Answer() {
	}
	
	public Answer(final String name, final String text, final String time) {
	        this.name = name;
	        this.text = text;
	        this.time = time;
	}
	
	public Answer(final String name, final String text) {
        this.name = name;
        this.text = text;
    }

	public Answer(String name) {
		this.name = name;
	}
	
	public String getText() {
        return text;
    }

    public String getTime() {
        return time;
    }

	public String getName() {
		return name;
	}
}
