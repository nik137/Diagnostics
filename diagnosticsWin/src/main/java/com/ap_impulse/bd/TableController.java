package com.ap_impulse.bd;

import javafx.application.Platform;
import javafx.beans.binding.Bindings;
import javafx.beans.property.FloatPropertyBase;
import javafx.beans.property.ReadOnlyFloatProperty;
import javafx.beans.property.SimpleFloatProperty;
import javafx.event.ActionEvent;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.image.Image;
import javafx.scene.layout.GridPane;
import javafx.stage.Stage;

import java.util.Objects;
import java.util.Optional;
import java.util.function.UnaryOperator;

import com.ap_impulse.Main;

/*�������� ������� ��� ���������� ��. ���������*/

public class TableController {

    public TableView  <ValuelistModel>          exampleTable; 
    public TableColumn<ValuelistModel, Integer> id_element_Column;
    public TableColumn<ValuelistModel, String>  name_Column;          
    public TableColumn<ValuelistModel, Float>   value_Column;         
    public TableColumn<ValuelistModel, String>  id_svg_Column;     
    
    public Button editButton;
    public Button deleteButton;

    public void initialize() {
        exampleTable.setItems(ValueListTable.getParametrs());
        id_element_Column.setCellValueFactory(new PropertyValueFactory<>("id_element"));
        value_Column.setCellValueFactory(new PropertyValueFactory<>("value"));
        name_Column.setCellValueFactory(new PropertyValueFactory<>("name"));
        id_svg_Column.setCellValueFactory(new PropertyValueFactory<>("id_svg"));
		
        editButton.disableProperty().bind(Bindings.isEmpty(exampleTable.getSelectionModel().getSelectedItems()));
        deleteButton.disableProperty().bind(Bindings.isEmpty(exampleTable.getSelectionModel().getSelectedItems()));
    }

    public void handleExitButtonClicked(ActionEvent event) {
        Platform.exit();
        event.consume();
    }
    
    public void addPerson(ActionEvent event) {
        Dialog<ValuelistModel> addPersonDialog = createPersonDialog(null);

        Optional<ValuelistModel> result = addPersonDialog.showAndWait();  
        System.out.println(result);
        
        result.ifPresent(parametr ->
                ValueListTable.insertParametr(
                		parametr.getValue(),
                		parametr.getName(),
                		parametr.getIdSVG()
                ));
        event.consume();
        System.out.println(result);
    }

    public void deletePerson(ActionEvent event) {
        for (ValuelistModel parametr : exampleTable.getSelectionModel().getSelectedItems()) {
            ValueListTable.delete(parametr.getId_element());
            //���� �������
        }
        event.consume();
    }

    public void editPerson(ActionEvent event) {
        if (exampleTable.getSelectionModel().getSelectedItems().size() != 1) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Error");
            alert.setHeaderText("Person editing error");
            alert.setContentText("One person must be selected when editing");
        } else {
            Dialog<ValuelistModel> dialog = createPersonDialog(exampleTable.getSelectionModel().getSelectedItem());
            Optional<ValuelistModel> optionalPerson = dialog.showAndWait();
            optionalPerson.ifPresent(ValueListTable::update);
        }
        event.consume();
    }

    private Dialog<ValuelistModel> createPersonDialog(ValuelistModel parametr) {
        //create the dialog itself
        Dialog<ValuelistModel> dialog = new Dialog<>();
        dialog.setTitle("Add Dialog");
        if(parametr==null){
            dialog.setHeaderText("Add a new parametr to the database");
        } else {
            dialog.setHeaderText("Edit a database record");
        }
        dialog.getDialogPane().getButtonTypes().addAll(ButtonType.OK, ButtonType.CANCEL);
        Stage dialogWindow = (Stage) dialog.getDialogPane().getScene().getWindow();
        dialogWindow.getIcons().add(new Image(Main.class.getResource("/EdenCodingIcon.png").toExternalForm()));

        //create the form for the user to fill in
        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));
        TextField valueElement = new TextField();
        valueElement.setPromptText("Value");
        TextField nameElement = new TextField();
        nameElement.setPromptText("Name");
        TextField numberSvgElement = new TextField();
        numberSvgElement.setPromptText("SVG");
        grid.add(new Label("Value:"), 0, 0);
        grid.add(valueElement, 1, 0);
        grid.add(new Label("Name:"), 0, 1);
        grid.add(nameElement, 1, 1);
        grid.add(new Label("SVG:"), 0, 2);
        grid.add(numberSvgElement, 1, 2);
        dialog.getDialogPane().setContent(grid);

        //disable the OK button if the fields haven't been filled in
        dialog.getDialogPane().lookupButton(ButtonType.OK).disableProperty().bind(
                Bindings.createBooleanBinding(() -> valueElement.getText().trim().isEmpty(), valueElement.textProperty())
                        .or(Bindings.createBooleanBinding(() -> nameElement.getText().trim().isEmpty(), nameElement.textProperty())
                                .or(Bindings.createBooleanBinding(() -> numberSvgElement.getText().trim().isEmpty(), numberSvgElement.textProperty())
                                )));

        //ensure only numeric input (integers) in age text field
        UnaryOperator<TextFormatter.Change> numberValidationFormatter = change -> {
            if (change.getText().matches("\\d+") || change.getText().equals("")) {
                return change; //if change is a number or if a deletion is being made
            } else {
                change.setText(""); //else make no change
                change.setRange(    //don't remove any selected text either.
                        change.getRangeStart(),
                        change.getRangeStart()
                );
                return change;
            }
        };

        valueElement.setTextFormatter(new TextFormatter<Object>(numberValidationFormatter));

        //make sure the dialog returns a Person if it's available
        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == ButtonType.OK) {
                int id = -1;
                if (parametr != null) id = parametr.getId_element();
                return new ValuelistModel(id, Float.valueOf(valueElement.getText()), nameElement.getText(), numberSvgElement.getText());
            }
            return null;
        });

        //if a record is supplied, use it to fill in the fields automatically
        if (parametr != null) {
        	nameElement.setText(parametr.getName());
            valueElement.setText(String.valueOf(parametr.getValue()));
            numberSvgElement.setText(parametr.getIdSVG());
        }
        return dialog;
    }
}