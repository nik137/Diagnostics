package diagnosticsWin;

import java.net.URL;
import java.util.ResourceBundle;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.layout.StackPane;
import javafx.scene.web.WebEngine;
import javafx.scene.web.WebView;

public class SplashController implements Initializable {

    @FXML
    private StackPane rootPane;
    
    @FXML private WebView webView; 
    /* ������ ����������� � ������������ ���������� WebEngine. �.�.embedded browser 
     * � ���������� JavaFX. ������� �� WebKit, ������������ CSS, JavaScript, DOM � HTML5.
     * ������������ ������� ���� � ��������� ������� ����������*/
    private WebEngine webEngine; 
    /*������ ��������������� ��������� ���-�������� � ������������ �������� ���-�������� 
     * �������� ���������� HTML � ������ � DOM ��� �� ��������� ������� JavaScript*/
    

    @Override
    public void initialize(URL url, ResourceBundle rb) {
    	
    	webEngine = webView.getEngine(); // �������� ������ WebEngine �� WebView ��������� ����� getEngine().
        URL url1 = getClass().getResource("/android_switch_string.svg");
        webEngine.load(url1.toExternalForm()); //������ ��������, ���������� ����� CSS and JavaScript.
    }
}