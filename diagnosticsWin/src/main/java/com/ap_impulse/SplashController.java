package com.ap_impulse;

import java.net.URL;
import java.util.ResourceBundle;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.layout.StackPane;
import javafx.scene.web.WebEngine;
import javafx.scene.web.WebView;

public class SplashController implements Initializable {
    @FXML
    public StackPane rootPane;
    
    @FXML private WebView webView; 
    /* An object that manages and displays the content of the "WebEngine". Those. embedded browser
     * to a JavaFX application. Based on "WebKit", supports CSS, JavaScript, DOM and HTML5.
     * Handles mouse events and some keyboard events*/
    private WebEngine webEngine; 
    /* The object directly loads a web page and maintains basic web properties
     * loading HTML content and accessing the DOM also executes JavaScript commands*/
    

    @Override
    public void initialize(URL url, ResourceBundle rb) {   	
    	webEngine = webView.getEngine(); // Get the WebEngine object from the WebView using the getEngine() method.
        URL url1 = getClass().getResource("/android_switch_string.svg");
        webEngine.load(url1.toExternalForm()); //Triggering an animation written with CSS and JavaScript.
    }
}