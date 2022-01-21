
package diagnosticsWin;

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.sun.javafx.webkit.WebConsoleListener; 
import javafx.animation.FadeTransition;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.concurrent.Worker;
import javafx.concurrent.Worker.State;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.input.MouseButton;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.StackPane;
import javafx.scene.web.WebEngine;
import javafx.scene.web.WebView;
import javafx.util.Duration;
import netscape.javascript.JSObject;

/*
 *restriction ограничение на подавление предупреждений об использовании нерекомендуемых или запрещенных ссылок
 * */
@SuppressWarnings("restriction")
public class MainSceneController implements Initializable {
	
	public String numberID = "";
	
	@FXML
	private WebView browser = new WebView(); 
    
    int answer_id = 0;
    boolean iclick = false;
	
    /*Храним мост в переменной  чтобы избежать Java GC*/
	private Bridge jl = new Bridge();
	
	
   public class Bridge { 
	   
       public String elementId() {    // отправляем обратно в свг
           System.out.println("receive id");
           return  numberID;
       }
       
       public String elementIdText() {
    	   if (numberID.startsWith("tspan")) {
    		   String answer = "Java Callback";
    		   iclick = true;   // флаг, который разрешает вывод контекстного меню
    		   answer+=' '+ numberID;
    		   return answer;
    	   } 
    	   return "Finish";
       }
   }
   
   @FXML
	private AnchorPane rootMain;
   	
	@FXML
	private AnchorPane root;
	//Обращение по имени 
	@FXML
    private Button mainButton;
	    
	    private WebEngine webEngine; 
	
      /*Инициализация элементов управления*/
	  public void initialize(URL location, ResourceBundle resources) {// initialize()для инициализации любых элементов управления
	        if (!Main.isSplashLoaded) {
	            loadSplashScreen();        
	        }
	        

	       /*Получаем объект WebEngine из WebView используя метод getEngine().*/
	       webEngine = browser.getEngine(); 
	       URL url = getClass().getResource("/function.svg");
	       webEngine.load(url.toExternalForm()); 
	       
	       /* getLoadWorker - Возвращает объект Worker, который отслеживает прогресса загрузки.После устанавливаем ссылку JSObject на «объект окна».
	        * jsobj теперь равен объекту окна, который представляет открытое окно в браузере  
	        * */
	       browser.getEngine().getLoadWorker().stateProperty().addListener(new ChangeListener<State>() {  
	           @Override
	           public void changed(ObservableValue ov, State oldState, State newState) {
	               if (newState == Worker.State.SUCCEEDED) {
	                   System.out.println("READY");
	                   JSObject jsobj = (JSObject) browser.getEngine().executeScript("window");
	                   /*Устанавливаем член объекта в JavaScript. Т.е. обращаемся из JavaScript к функции Java*/
	                   jsobj.setMember("bridge", jl);
	               }
	           }
	       });
	      
	       /*
	        * Прослушиватель сообщений из консоли WEB - import com.sun.javafx.webkit.WebConsoleListener
	        */
	       WebConsoleListener.setDefaultListener((webView, message, lineNumber, sourceId) -> {
	     	    System.out.println(message + "[at " + lineNumber + "]");
	     	    numberID =  message;
	     	});
	       
	         /*Disable the default context menu*/
	         browser.setContextMenuEnabled(false);
	         
	         /*Подключаем свое меню*/
	         createContextMenu(browser); // подключаем свое кастомное меню
	  }
	  	  
	  
	   /* 
	    * Кастомное контекстное меню. С обрабочтком события по клику правой кнопкой мыши
	    * и левой двойным щелчком. 
	    */
	   private void createContextMenu(WebView webView) {
	       ContextMenu contextMenu = new ContextMenu();
	       ContextMenu contextMenu2 = new ContextMenu();
	       MenuItem reload = new MenuItem("Reload");
	       reload.setOnAction(e -> webView.getEngine().reload());
	       MenuItem test = new MenuItem("Test executeScript ");
	       test.setOnAction(e ->{ System.out.println("Test executeScript");
	                              browser.getEngine().executeScript("GreenPath()");
	                            });
	       MenuItem CallbackText = new MenuItem("CallbackText");
	       CallbackText.setOnAction(e ->{ System.out.println("CallbackText");
	                                      browser.getEngine().executeScript("doCallbackText()");
	                                      iclick = false;
	                                    });
	      	       
	       contextMenu.getItems().addAll(reload, test);
	       contextMenu2.getItems().addAll(CallbackText);
	        	       
	       webView.setOnMousePressed(e -> {    	   
	           if (e.getButton() == MouseButton.SECONDARY) {
	               contextMenu.show(webView,  e.getScreenX(), e.getScreenY()); 
	           }else {
	               contextMenu.hide();
	           } 
	           if ((e.getClickCount() == 2)/* && iclick == true*/) {	          
	        	   contextMenu2.show(webView, e.getScreenX(), e.getScreenY());     
	           }else {
	               contextMenu2.hide();
	           }
	       });
	   }
	   
	  
	  /*Анимация окна загрузки с затуханием*/ 
	  private void loadSplashScreen() {
	        try {
	            Main.isSplashLoaded = true;
	            StackPane pane = FXMLLoader.load(getClass().getResource(("/splash_svg.fxml")));
	            rootMain.setPrefSize(300, 300);
	            root.setPrefSize(300, 300);
	            root.getChildren().setAll(pane);

	            //Load splash screen with fade in effect
	            FadeTransition fadeIn = new FadeTransition(Duration.seconds(3), pane);
	            fadeIn.setFromValue(0);
	            fadeIn.setToValue(1);
	            fadeIn.setCycleCount(1);

	            //Finish splash with fade out effect
	            FadeTransition fadeOut = new FadeTransition(Duration.seconds(3), pane);
	            fadeOut.setFromValue(1);
	            fadeOut.setToValue(0);
	            fadeOut.setCycleCount(1);

	            fadeIn.play();

	            //After fade in, start fade out
	            fadeIn.setOnFinished((e) -> { 
	                fadeOut.play();
	            });
	            
	            //After fade out, load actual content
	            fadeOut.setOnFinished((e) -> {
	                try {	                                	
	                   AnchorPane parentContent = FXMLLoader.load(getClass().getResource(("/main.fxml")));
	                   root.getChildren().setAll(parentContent);
	                } catch (IOException ex) {
	                    Logger.getLogger(MainSceneController.class.getName()).log(Level.SEVERE, null, ex);
	                }
	            });

	        } catch (IOException ex) {
	            Logger.getLogger(MainSceneController.class.getName()).log(Level.SEVERE, null, ex);
	        }
	    }
	  
	    public void updateColor(String newColor) {
	        root.setStyle("-fx-background-color:" + newColor);
	    }

	    @FXML
	    private void buttonClicked() {
	        mainButton.setText("Click me again!");
	     }  
}

 