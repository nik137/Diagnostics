package com.ap_impulse.websocket;

import java.text.SimpleDateFormat;
import java.util.Date;

import org.springframework.messaging.handler.annotation.MessageMapping;
import org.springframework.messaging.handler.annotation.SendTo;
import org.springframework.stereotype.Controller;

/**
* AnswerController.java Контролер що приймає запит - json - об'єкт від клієнта. 
* <p>
* Об'єкт що приймає запит - json - об'єкт від клієнта. (ім'я змінної, та якійсь текст)  
* @param  name, text
* @return name, text
*/ 
@Controller
public class AnswerController {
	@MessageMapping("/answer") 
	@SendTo("/topic/answers") 
	public Answer answer(ValuesMessage message) throws Exception {
		System.out.println(message.getName());
		System.out.println(message.getText());
		Thread.sleep(1000); 
		final String time = new SimpleDateFormat("HH:mm").format(new Date());
		return new Answer(message.getName(), message.getText(), time);
	}
}
