
package diagnosticsWin;

import java.io.IOException;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.List;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.sun.javafx.webkit.WebConsoleListener;
import javafx.animation.FadeTransition;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.concurrent.ScheduledService;
import javafx.concurrent.Task;
import javafx.concurrent.Worker;
import javafx.concurrent.Worker.State;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.Label;
import javafx.scene.control.MenuItem;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.input.MouseButton;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.StackPane;
import javafx.scene.web.WebEngine;
import javafx.scene.web.WebView;
import javafx.stage.Stage;
import javafx.util.Duration;
import netscape.javascript.JSObject;
import javafx.scene.control.Alert;

/*"restriction" restriction to suppress warnings about the use of deprecated or prohibited links*/
@SuppressWarnings("restriction")
public class MainSceneController implements Initializable {
	
	List<String> indValue =  new ArrayList<>();
	Collection<Valuelist> peopleList =  new ArrayList<>();
	ValuesApplication valuesappl = new ValuesApplication();  
	Valuelist val = new Valuelist();
	String answer = null;
	String answer_id_svg = null;
	List <MenuItem> listValueMenu;
	  
	/*To populate a menu with variables*/
	private MenuItem it0,it1,it2,it3,it4,it5,it6,it7,it8,it9,it10,it11,it12,it13,it14,it15,it16,it17,it18,it19,it20;
	MenuItem[] valueitems = {it0,it1,it2,it3,it4,it5,it6,it7,it8,it9,it10,it11,it12,it13,it14,it15,it16,it17,it18,it19,it20};
			
	public String numberID = "";
	
	@FXML
	private WebView browser = new WebView(); 
    
    int answer_id = 0;
    boolean iclick = false;
	
    /*Store the bridge in a variable to avoid Java GC*/
	private Bridge jl = new Bridge();
	
    public class Bridge { 
       public String elementId() {    //Send back to svg
           System.out.println("receive id");
           return  numberID;
       }   
       
       public String elementIdText() {  
    	   if (numberID.startsWith("tspan")) {
    		   /*Flag that allows the context menu to be displayed*/
    		   iclick = true;  
    		   answer_id_svg = numberID;
    		   return answer;
    	   } 
    	   return "Finish";
       }
       
       public String Id_Text() {	   
    	   answer_id++;
		   return Integer.toString(answer_id);
       }
   }
    
    /*Label for periodic output of any information*/
    private @FXML Label timerNumber;   
    
    
    
   
    @FXML
	private AnchorPane rootMain;
   	
	@FXML
	private AnchorPane root;
	//Address by name 
	@FXML
    private Button mainButton;
	    
	    private WebEngine webEngine; 
	
      /*Control initialization*/
	  public void initialize(URL location, ResourceBundle resources) {//"initialize()" to initialize any controls
	        if (!Main.isSplashLoaded) {
	            loadSplashScreen();        
	        }
	        
	        //To output to timestamp cyclically
	        UpdateLabel service = new UpdateLabel(timerNumber);
	        service.setPeriod(Duration.seconds(5)); // The interval between executions.
	        service.start();

	       /*Get the "WebEngine" object from the "WebView" using the "getEngine()" method*/
	       webEngine = browser.getEngine(); 
	       URL url = getClass().getResource("/function.svg");
	       webEngine.load(url.toExternalForm()); 
	       
	       /* getLoadWorker - Returns a "Worker" object that tracks the progress of loading. After that, we set the "JSObject"
	        * reference to the "window object". "jsobj" is now equal to the window object which represents the open window in the browser */
	       browser.getEngine().getLoadWorker().stateProperty().addListener(new ChangeListener<State>() {  
	           @Override
	           public void changed(ObservableValue ov, State oldState, State newState) {
	               if (newState == Worker.State.SUCCEEDED) {
	                   System.out.println("READY");
	                   JSObject jsobj = (JSObject) browser.getEngine().executeScript("window");
	                   /*Set an object member in JavaScript. Those accessing a Java function from JavaScript*/
	                   jsobj.setMember("bridge", jl);
	               }
	           }
	       });
	      
	       /*
	        *Console message listener WEB - import com.sun.javafx.webkit.WebConsoleListener
	        */
	       WebConsoleListener.setDefaultListener((webView, message, lineNumber, sourceId) -> {
	     	    System.out.println(message + "[at " + lineNumber + "]");
	     	    numberID =  message;
	     	});
	       
	       //-----------------------------------------------------------------------------
           valuesappl.addElements(peopleList);   
	       /*Populate the list*/
	       for (Valuelist item : peopleList) {   
	     	    indValue.add(item.getName());  
	       }
	       
	       System.out.println(indValue);
	       /*Create menus*/
	       int i = 0;
	       for(String item : indValue) {
	     	  valueitems[i++] =  new MenuItem(item);
	       }
	       
	         /*Disable the default context menu*/
	         browser.setContextMenuEnabled(false);
	         
	         /*We connect our menu*/
	         createContextMenu(browser); // Connect your custom menu
	  }
	  	  
	  
	   /* Custom context menu. With right click event handling and left double click. */
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
	       
	       MenuItem dataBase = new MenuItem("Database");
	       dataBase.setOnAction(e -> { 	   								
	    	   if (Database.isOK()) {  
						Stage newStage = new Stage();
				        Parent rootDatabase;
					try {
						rootDatabase = FXMLLoader.load(getClass().getResource("/sqLiteView.fxml"));
					    Scene scene = new Scene(rootDatabase); // Scene отображает содержание сцены (stage)  
					        newStage.setScene(scene); // загружаем компоненты в окно
					        newStage.setTitle("Database"); 
					        newStage.show(); // С помощью метода show объект Stage отображается на экране устройства.  
					} catch (IOException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
	           } else {	        	   
	               Alert alert = new Alert(AlertType.ERROR);
	   			   alert.setTitle("Database error");
	   			   alert.setHeaderText("Could not load database");
	   			   alert.setContentText("Error loading SQLite database. See log. Quitting");
	   			   alert.showAndWait();
	               Platform.exit(); 
	           }
	       							});
	       
	       /*Item click eventт*/
		   for (MenuItem item : valueitems) {
	         if (item != null) item.setOnAction(e -> { System.out.println(item.getText());
	         										   answer = item.getText();                
	         							    		   /*Here we assign id svg to the collection*/
	         							    		   if (answer != "Finish" && answer != null ) { 
	         							    			   for (Valuelist items1 : peopleList) {   
	         							    				  if(items1.getName().equals(answer)) {
	         							    					  answer = answer + " = " + items1.getId();
	         	         							    		  browser.getEngine().executeScript("doCallbackText()");
	         							    					  items1.setIdSVG(answer_id_svg);
	         							    				  }
	         							    			   }
	         							    			  System.out.println(peopleList);	    
	         							    		   }
	         							    		   iclick = false;
	         										    });
	         else break; 
	       }
	       contextMenu.getItems().addAll(reload, test, dataBase);
	       contextMenu2.getItems().addAll(CallbackText);
	       for (MenuItem item : valueitems) {
	         if (item != null) contextMenu2.getItems().addAll(item);
	         else break; 
	       }
	        	       
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
	   
	  /*Fade loading window animation*/ 
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
	    
	    
	    /*A class that extends a flow control service. Accepts a label in the graphic element constructor.
	      Has a runnable task to display the time/update of an element after a given time.*/
	    class UpdateLabel extends ScheduledService<Void> {
	    	private Label label;
	        public UpdateLabel(Label label){
	    	    this.label = label;
	        }
			@Override
	    	protected Task<Void> createTask(){
	    	    return new Task<Void>(){
	    	        @Override
	    	        protected Void call(){
	    	            Platform.runLater(() -> {
	    	            	Calendar time = Calendar.getInstance();
	    	            	SimpleDateFormat simpleDateFormat = new SimpleDateFormat("HH:mm:ss");
	    	            	label.setText(simpleDateFormat.format(time.getTime()));
	    	            });
	    	            return null;
	    	        }
	    	    };
	    	}
	    }
}

 