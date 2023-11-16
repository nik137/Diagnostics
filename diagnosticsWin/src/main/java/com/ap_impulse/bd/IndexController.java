package com.ap_impulse.bd;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.RequestMapping;

@Controller
public class IndexController {

	@Autowired
	DBService dbservice;

	@RequestMapping("/")
	public String index(Model model) {
		model.addAttribute("valueList", dbservice.getData());
		//Print read records:
		dbservice.getData().forEach(System.out::println);
		return "index";
	}
}
