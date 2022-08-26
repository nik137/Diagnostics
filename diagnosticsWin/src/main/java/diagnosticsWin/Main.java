package diagnosticsWin;

import java.io.IOException;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class Main extends Application { 
    public static Boolean isSplashLoaded = false;
    @Override
    public void start(Stage stage) throws IOException { 
    	/*FXMLLoader - creates an instance of the corresponding controller (specified in the FXML file via fx:controller),
         *calling the first default constructor and then initialize*/
        Parent root = FXMLLoader.load(getClass().getResource("/main.fxml"));
        /*Displays the contents of the stage (stage)*/
        Scene scene = new Scene(root);
        /*Adding a style to the app's background*/
        scene.getStylesheets().add(getClass().getResource("/ApplicationStyle.css").toExternalForm());
        /*Loading components into the window*/
        stage.setScene(scene); 
        stage.setTitle("Genuine Coder"); 
        /*Stage is displayed on the device screen.*/
        stage.show(); 
    }
    /*The "launch" method starts the JavaFX application. After that, the "start" method is called*/
    public static void main(String[] args) {
        launch(args);
    }      
}
