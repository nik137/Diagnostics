package com.ap_impulse;

import com.ap_impulse.websocket.AnswerController;

/**
* ParsingComponent.java - обробка компонентів відповіді/запиту в моделі ValuesMessage. 
* <p>
* Обробка компонентів відповіді/запиту в моделі ValuesMessage. 
* @param  name, text
* @return name, text
*/ 
public class ParsingComponent {
	
	public ParsingComponent() {
		// TODO Auto-generated constructor stub
	}

	// Парсінг компоненти параметра її значення та id
	public void parsingName(String name) {
		String namelocal = name; 										   				// для ім'я
		String valuelocal = name;										   				// для значення
		// Парсім id 
		if(namelocal.contains("=")) namelocal = namelocal.split("=")[0];   				// видаляємо все після символу "=" 
		namelocal = namelocal.replaceAll("=", ""); 						   				// повино залишиться тількі id - назва параметру, наприклад "N#"
		if (namelocal == null) return; 									   				// на всякій випадок
		
		// Парсім значення
		if(valuelocal.contains("=")) valuelocal = valuelocal.replaceFirst(".*=", ""); 	// видаляємо все до символу "=" 
	    valuelocal = valuelocal.replaceAll("=", ""); 								  	// повинно залишиться тількі значення параметру
		System.out.println("Parsing:name:"+namelocal+ ",value:" +valuelocal);			// Parsing:name:N#-макс'+',value:120
		
		//Викликаємо колекцію з параметрами, шукаємо назву і вставляємо значення 
		for (Valuelist items1 : ValuesApplication.componentValueList) {   // здесь берем всю коллекцию пар ключей и прогоняем по парам
			if(items1.getName().equals(namelocal) ) {
				items1.setId(Float.parseFloat(valuelocal.replaceAll(" ", "")));
				System.out.println("Collection:" + items1); // Collection:Valuelist [id_element=2, id=120.0, name=N#-макс'+', id_svg=null]
			}
		}
	}
	
	// Парсінг компоненти текста з командами
	public String parsingText(String text) {
		String answer= null;
		if(text.equals("Text")) {
			AnswerController.startTraining = false;   //дозвіл пошуку через нейромережу
			return answer = "Немає даних для аналізу";
		}
		AnswerController.startTraining = true;
		return text;
	}
	

}
