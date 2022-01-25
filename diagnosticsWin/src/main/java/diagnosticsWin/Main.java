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
    	/*FXMLLoader - c������ ��������� ���������������� ����������� (�������� � FXML����� ����� fx:controller), ������� ������ ����������� �� ��������� , � ����� initialize*/
        Parent root = FXMLLoader.load(getClass().getResource("/main.fxml"));
        /*���������� ���������� ����� (stage)*/
        Scene scene = new Scene(root);
        /*��������� ���������� � ����*/
        stage.setScene(scene); 
        stage.setTitle("Genuine Coder"); 
        /*Stage ������������ �� ������ ����������.*/
        stage.show(); 
    }
    /*����� launch ��������� ���������� JavaFX. ����� ����� ���������� ����� start*/
    public static void main(String[] args) {
        launch(args);
    }      
}
