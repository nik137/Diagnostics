package diagnosticsWin;

import java.io.IOException;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
//Главный класс приложения JavaFX должен наследоваться от класса javafx.application.Application
public class Main extends Application {   //Запуск в мейне
    public static Boolean isSplashLoaded = false;
    @Override
    public void start(Stage stage) throws IOException { 
        Parent root = FXMLLoader.load(getClass().getResource("/main.fxml"));
        //root корневой компонент пользовательского интерфейса 
        Scene scene = new Scene(root); // Scene отображает содержание сцены (stage)
        
        stage.setScene(scene); // загружаем компоненты в окно
        stage.setTitle("Genuine Coder"); 
        // stage.setWidth(300);          // установка ширины окна
        // stage.setHeight(250);         // установка длины окна
        stage.show(); // С помощью метода show объект Stage отображается на экране устройства.       
    }
    //Метод launchзапускает приложение JavaFX. После этого вызывается метод start
    public static void main(String[] args) {
        launch(args);
    }        
}


