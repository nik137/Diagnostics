package diagnosticsWin;

import java.io.IOException;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
//������� ����� ���������� JavaFX ������ ������������� �� ������ javafx.application.Application
public class Main extends Application {   //������ � �����
    public static Boolean isSplashLoaded = false;
    @Override
    public void start(Stage stage) throws IOException { 
        Parent root = FXMLLoader.load(getClass().getResource("/main.fxml"));
        //root �������� ��������� ����������������� ���������� 
        Scene scene = new Scene(root); // Scene ���������� ���������� ����� (stage)
        
        stage.setScene(scene); // ��������� ���������� � ����
        stage.setTitle("Genuine Coder"); 
        // stage.setWidth(300);          // ��������� ������ ����
        // stage.setHeight(250);         // ��������� ����� ����
        stage.show(); // � ������� ������ show ������ Stage ������������ �� ������ ����������.       
    }
    //����� launch��������� ���������� JavaFX. ����� ����� ���������� ����� start
    public static void main(String[] args) {
        launch(args);
    }        
}


