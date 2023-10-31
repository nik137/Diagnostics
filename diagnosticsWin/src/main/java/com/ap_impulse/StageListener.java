package com.ap_impulse;

import java.io.IOException;
import java.net.URL;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.ApplicationContext;
import org.springframework.context.ApplicationListener;
import org.springframework.core.io.Resource;
import org.springframework.stereotype.Component;

import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

@Component
public class StageListener implements ApplicationListener<StageReadyEvent>{

	private final String applicationTitle;
	private final Resource fxml;
	private final ApplicationContext ac;
	
	StageListener(@Value("${spring.application.ui.title}") String applicationTitle,
			      @Value("classpath:/main.fxml") Resource resourse, ApplicationContext ac){
		this.applicationTitle = applicationTitle;
		fxml = resourse;
		this.ac = ac;
	}
	
	@Override
	public void onApplicationEvent(StageReadyEvent event) {
		// TODO Auto-generated method stub
		try {
			Stage stage = event.getStage();
			URL url = fxml.getURL();
			FXMLLoader fxmlLoader = new FXMLLoader(url);
			fxmlLoader.setControllerFactory(ac::getBean );
			Parent root = fxmlLoader.load();
			Scene scene = new Scene(root, 600, 600);
			
			stage.setScene(scene);
			stage.setTitle(applicationTitle);
			stage.show();
		
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			throw new RuntimeException(e);
		}      
	}

}










