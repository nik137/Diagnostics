package com.ap_impulse.bd;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Service;

@Service
public class DBService {
	@Autowired
	private JdbcTemplate jdbcTemplate;
	public List<ValuelistModel> getData(){
		String selectSql = "SELECT * FROM ValueList";
		List<ValuelistModel> insertedRows = jdbcTemplate.query(selectSql,
				(resultSet, rowNum) -> new ValuelistModel(resultSet.getInt("numberElement"),resultSet.getFloat("valueElement"),resultSet.getString("nameElement"),resultSet.getString("numberSvgElement")));
		return insertedRows;
	}
}
