
package diagnosticsWin;
import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;


import javafx.animation.FadeTransition;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.StackPane;
import javafx.util.Duration;

public class MainSceneController implements Initializable {
	@FXML
	private AnchorPane root;
	//Обращение по имени 
	@FXML
    private Button mainButton;

	  public void initialize(URL url, ResourceBundle rb) {// initialize()для инициализации любых элементов управления
	        if (!Main.isSplashLoaded) {
	            loadSplashScreen();  
	        }
	    }
	
    @FXML
    private void buttonClicked() {
        mainButton.setText("Click me again!");
     }
   
    private void loadSplashScreen() {
        try {
            Main.isSplashLoaded = true;
            //Load splash screen view FXML
            StackPane pane = FXMLLoader.load(getClass().getResource(("/splash_svg.fxml")));
            
            
            //Add it to root container (Can be StackPane, AnchorPane etc)
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
}
