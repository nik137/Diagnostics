package com.ap_impulse;

import org.springframework.boot.builder.SpringApplicationBuilder;
import org.springframework.context.ApplicationContextInitializer;
import org.springframework.context.ApplicationEvent;
import org.springframework.context.ConfigurableApplicationContext;
import org.springframework.context.support.GenericApplicationContext;

import javafx.application.Application;
import javafx.application.HostServices;
import javafx.application.Platform;
import javafx.stage.Stage;

public class Main extends Application { 
	//For start spring context
    private ConfigurableApplicationContext context;
    public static Boolean isSplashLoaded = false;
    
    @Override
	public void init() throws Exception {
		// TODO Auto-generated method stub			
		ApplicationContextInitializer<GenericApplicationContext> initializer =	
				ac ->{
				// TODO Auto-generated method stub
				ac.registerBean(Application.class, ()-> Main.this);
				ac.registerBean(Parameters.class, ()-> getParameters());
				ac.registerBean(HostServices.class, this::getHostServices);
		};
		
		context = new SpringApplicationBuilder()
				.sources(DiagnosticsWinApplication.class)
				.initializers(initializer)
				.run(getParameters().getRaw().toArray(new String[0]));
	}
    
    //Start JavaFX scene for event
    @Override
    public void start(Stage primaryStage) throws Exception {
        context.publishEvent(new StageReadyEvent(primaryStage));
    }
    
    //Closing the Application Context
    @Override
    public void stop() {
        context.close();
        Platform.exit();
    }
}

class StageReadyEvent extends ApplicationEvent {
	public Stage getStage() {
		return Stage.class.cast(getSource());
	}
	public StageReadyEvent(Stage source) {
		super(source);
		// TODO Auto-generated constructor stub
	}
}
