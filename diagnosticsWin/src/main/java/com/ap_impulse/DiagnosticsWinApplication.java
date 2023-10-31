package com.ap_impulse;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.jdbc.core.JdbcTemplate;

import javafx.application.Application;

@SpringBootApplication
public class DiagnosticsWinApplication implements CommandLineRunner {
	@Autowired
	private JdbcTemplate jdbcTemplate;

	public static void main(String[] args) {
		Application.launch(Main.class, args);
	}

	@Override
	public void run(String... args) throws Exception {
		//Read records (for test):
		List<ValuelistModel> valuelistModel = jdbcTemplate.query("SELECT * FROM ValueList",
		(resultSet, rowNum) -> new ValuelistModel (resultSet.getInt("numberElement"),resultSet.getFloat("valueElement"),resultSet.getString("nameElement"),resultSet.getString("numberSvgElement")));
			
		//Print read records:
		valuelistModel.forEach(System.out::println);
	}

}
