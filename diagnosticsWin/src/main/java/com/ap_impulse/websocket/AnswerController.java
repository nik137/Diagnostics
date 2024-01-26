package com.ap_impulse.websocket;

import java.text.SimpleDateFormat;
import java.util.Date;

import org.springframework.messaging.handler.annotation.MessageMapping;
import org.springframework.messaging.handler.annotation.SendTo;
import org.springframework.stereotype.Controller;

import com.ap_impulse.ParsingComponent;
import com.ap_impulse.ML.WordSearch;

/**
* AnswerController.java Контролер що приймає запит - json - об'єкт від клієнта. 
* <p>
* Об'єкт що приймає запит - json - об'єкт від клієнта. (ім'я змінної, та якійсь текст)  
* @param  name, text
* @return name, text
*/ 
@Controller
public class AnswerController {
	// Запускається один раз при старті програми. Проект робе на UTF-8, словник повинен відповідати цьому кодуванню
	WordSearch wordsearch = new WordSearch(); // в конструкторі запускається навчання
	// Парсінг - для розпізнавання компонентів
	ParsingComponent parsingComponent = new ParsingComponent();
	public static boolean startTraining = false;
	@MessageMapping("/answer") 
	@SendTo("/topic/answers") 
	public Answer answer(ValuesMessage message) throws Exception {
		String answer = "";
		System.out.println(message.getName());
		System.out.println(message.getText());
		parsingComponent.parsingName(message.getName());           // записує дані у внутрішне сховище
		answer = parsingComponent.parsingText(message.getText());  // аналізує команду, текст, там же дає дозвіл на пошук через нейромережу
		//wordsearch.training();
		if(startTraining == true) {
			answer = wordsearch.SearchComparision(answer); // кожен при відповіді на один Json
			startTraining = false;
		}
		Thread.sleep(1000); 
		final String time = new SimpleDateFormat("HH:mm").format(new Date());
		return new Answer(message.getName(), answer, time);
	}
}
