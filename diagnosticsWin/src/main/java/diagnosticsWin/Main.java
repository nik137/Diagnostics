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
    	/*FXMLLoader - cоздает экземпляр соответствующего контроллера (заданный в FXMLфайле через fx:controller), вызывая первый конструктор по умолчанию , а затем initialize*/
        Parent root = FXMLLoader.load(getClass().getResource("/main.fxml"));
        /*отображает содержание сцены (stage)*/
        Scene scene = new Scene(root);
        /*загружаем компоненты в окно*/
        stage.setScene(scene); 
        stage.setTitle("Genuine Coder"); 
        /*Stage отображается на экране устройства.*/
        stage.show(); 
    }
    /*Метод launch запускает приложение JavaFX. После этого вызывается метод start*/
    public static void main(String[] args) {
        launch(args);
    }      
}
