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
    /* Объект управляющий и отображающий содержимое WebEngine. Т.е.embedded browser 
     * в приложение JavaFX. Основан на WebKit, поддерживает CSS, JavaScript, DOM и HTML5.
     * Обрабатывает события мыши и некоторые события клавиатуры*/
    private WebEngine webEngine; 
    /*Объект непосредственно загружает веб-страницу и поддерживает основные веб-свойства 
     * загрузки содержания HTML и доступ в DOM так же выполняет команды JavaScript*/
    

    @Override
    public void initialize(URL url, ResourceBundle rb) {
    	
    	webEngine = webView.getEngine(); // Получить объект WebEngine из WebView используя метод getEngine().
        URL url1 = getClass().getResource("/android_switch_string.svg");
        webEngine.load(url1.toExternalForm()); //Запуск анимации, происанной через CSS and JavaScript.
    }
}