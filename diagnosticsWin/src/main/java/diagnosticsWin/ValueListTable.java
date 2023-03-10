package diagnosticsWin;

import java.sql.*;
import java.util.Optional;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

/*Читання параметрів із БД. Зв`язок параметрів з БД*/
public class ValueListTable {
    private static final String tableName 		  = "ValueList";	      
    private static final String id_element_Column = "numberElement";
    private static final String value_Column      = "valueElement";
    private static final String name_Column       = "nameElement";
    private static final String id_svg_Column     = "numberSvgElement";
    private static final ObservableList<ValuelistModel> parametrs;  	  

    static {
    	parametrs = FXCollections.observableArrayList(); 
        updateParametrsFromDB();  						 
    }

    public static ObservableList<ValuelistModel> getParametrs() {  
    	System.out.println(FXCollections.unmodifiableObservableList(parametrs));
        return FXCollections.unmodifiableObservableList(parametrs); 
    }

    //---------------------------Читання з БД- виконується при відкриті БД (таблиці)--------------------------------------------------
    private static void updateParametrsFromDB() { 		// Функція читання з БД
        String query = "SELECT * FROM " + tableName;    // Запит - читаємо все з таблиці
    
        try (Connection connection = Database.connect()) {
            PreparedStatement statement = connection.prepareStatement(query);
            ResultSet rs = statement.executeQuery(); 
            parametrs.clear();                      
            while (rs.next()) {						
            	parametrs.add(new ValuelistModel(    
            			rs.getInt(id_element_Column),		  
                        rs.getFloat(value_Column),
                        rs.getString(name_Column),
                        rs.getString(id_svg_Column)));
            }
        } catch (SQLException e) {
          /*  Logger.getAnonymousLogger().log(
                    Level.SEVERE,
                    LocalDateTime.now() + ": Could not load Parametrs from database ");*/
            parametrs.clear();
        }
    }

    /*----------------------------Оновити таблицю з БД-викликається при натискані кнопки Edit (редагування), через контролер-------------------------------------------------------*/
    public static void update(ValuelistModel newValuelist) {
        int rows = CRUDHelper.update( 																
                tableName,																			
                new String[]{value_Column, name_Column, id_svg_Column},                               
                new Object[]{newValuelist.getValue(), newValuelist.getName(), newValuelist.getIdSVG()},
                new int[]{Types.FLOAT, Types.VARCHAR, Types.VARCHAR},								
                id_element_Column,                                                                   
                Types.INTEGER,                                                                      
                newValuelist.getId_element()                                                            
        );
        //------------------------------------------------------------------------------------------------

        if (rows == 0)
            throw new IllegalStateException("Parametr to be updated with id " + newValuelist.getId_element() + " didn't exist in database");
       		Optional<ValuelistModel> optionalParametr = getParametr(newValuelist.getId_element());
            optionalParametr.ifPresentOrElse(oldValuelist -> {     
            parametrs.remove(oldValuelist);
            parametrs.add(newValuelist);
        }, () -> {
            throw new IllegalStateException("Parametr to be updated with id " + newValuelist.getId_element() + " didn't exist in database");
        });
    }

    //--------------------Функція вставити параметр, визивається при додаванні параметрів ------------------------------------------
    public static void insertParametr(Float value, String name, String id_svg) {
        //update database
        int id_elements= (int) CRUDHelper.create(
                tableName,
                new String[]{"valueElement", "nameElement", "numberSvgElement"},
                new Object[]{value, name, id_svg},
                new int[]{Types.FLOAT, Types.VARCHAR, Types.VARCHAR});
        //INSERT INTO ValueList (valueElement, nameElement, numberSvgElement)  VALUES (1.0, '1', '1');
        //update cache
        parametrs.add(new ValuelistModel(
        		id_elements,
        		value,
                name,
                id_svg                 
        ));
        System.out.println(parametrs); 
    }
    //----------------------------------------------------------------------------------------------------------------------------

    public static void delete(int id) {
        CRUDHelper.delete(tableName, id);
        //update cache
        Optional<ValuelistModel> parametr = getParametr(id);
        parametr.ifPresent(parametrs::remove);
    }

    public static Optional<ValuelistModel> getParametr(int id) {
        for (ValuelistModel parametr : parametrs) {
            if (parametr.getId_element() == id) return Optional.of(parametr);
        }
        return Optional.empty();
    }
}