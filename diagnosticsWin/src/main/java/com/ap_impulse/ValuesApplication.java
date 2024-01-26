/*Storing data while the application is running.*/
package com.ap_impulse;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Objects;

/**
 * Created by ������� on 04.10.2019.
 */

public class ValuesApplication {   
	
	// For data from web
	public static Collection<Valuelist> componentValueList =  new ArrayList<>(); 
	
	// Constructor
	ValuesApplication(){
		    System.out.println(" --- Constructor ValuesApplication: created collection --- ");
		    	addElements(componentValueList); //заповнюємо статичну колекцію
    }
   
    /*private static - a field common to all instances of the class, which can only be accessed from the methods of this class.*/
    private static Float r_Z_SkMax_M = (float) 0.0;        
    private static Float r_Z_SkMax_P = (float) 0.0;        
    private static Float ZSkA = (float) 0.0;               
    private static Float Z_Skor = (float) 0.0;             
    private static Float r_Mashtab_ZSkA = (float) 0.0;     
    private static Float r_T_ZISkorP_Razg = (float) 0.0;    
    private static Float r_T_ZISkorP_Torm = (float) 0.0;    
    private static Float r_T_ZISkorP_Fors = (float) 0.0;    
    private static Float ZISkor = (float) 0.0;              
    private static Float Ud = (float) 0.0;    
    private static Float E = (float) 0.0;     
    private static Float OuRegS_dop_kod = (float) 0.0;      
    private static Float ZIDN = (float) 0.0;               
    private static Float Id_dop_kod = (float) 0.0;         
    private static Float Skor = (float) 0.0;               
    private static Float S_Alfa = (float) 0.0;             
    private static Float S_Alfa_Old = (float) 0.0;         
    private static Float V_Fv_zad = (float) 0.0;           

    public void deleteElements(Collection<?> collection) {		
		collection.clear();
	}
    
    // 1 - порядковий номер компоненти, r_Z_SkMax_M - сама зміна в системі, N#-max'-' - назва компоненти , а також її id на веб
    // "null" - резерв 
    public void addElements(Collection<Valuelist> collection) {		
		collection.add(new Valuelist(1    ,r_Z_SkMax_M       	    ,"N#-макс'-'"       		,"null"    			));
		collection.add(new Valuelist(2    ,r_Z_SkMax_P        		,"N#-макс'+'"       		,"null"    			));
		collection.add(new Valuelist(3    ,ZSkA               		,"N#A"             			,"null"    			));
		collection.add(new Valuelist(4    ,Z_Skor             		,"N#"              			,"null"    			));
		collection.add(new Valuelist(5    ,Skor              	    ,"N"               			,"null"    			));
		collection.add(new Valuelist(6    ,r_Mashtab_ZSkA     		,"N#-масштаб"      			,"null"    			));
		collection.add(new Valuelist(7    ,r_T_ZISkorP_Torm   		,"ТемпЗІ-РШ'-'"          	,"null"    			));
		collection.add(new Valuelist(8    ,r_T_ZISkorP_Fors   		,"ТемпЗІ-РШ's'"          	,"null"    			));
		collection.add(new Valuelist(9    ,r_T_ZISkorP_Razg   		,"ТемпЗІ-РШ'+'"          	,"null"    			));
		collection.add(new Valuelist(10   ,ZISkor           		,"N#R"              	    ,"null"    			));
	
	}
	
	
    public Float getZSkA() {return ZSkA;}
    public void setZSkA(Float ZSkA) {this.ZSkA = ZSkA;}
    public Float getZ_Skor() {return Z_Skor;}
    public void setZ_Skor(Float Z_Skor) {this.Z_Skor = Z_Skor;}
    public Float get_r_Mashtab_ZSkA() {return r_Mashtab_ZSkA;}
    public void set_r_Mashtab_ZSkA(Float r_Mashtab_ZSkA) {this.r_Mashtab_ZSkA = r_Mashtab_ZSkA;}

    public Float getr_Z_SkMax_M() {
        return r_Z_SkMax_M;
    }
    public void setr_Z_SkMax_M(Float r_Z_SkMax_M) {this.r_Z_SkMax_M = r_Z_SkMax_M;}

    public Float getr_Z_SkMax_P() {
        return r_Z_SkMax_P;
    }
    public void setr_Z_SkMax_P(Float r_Z_SkMax_P) {this.r_Z_SkMax_P = r_Z_SkMax_P;}
    public Float getr_T_ZISkorP_Razg() {return r_T_ZISkorP_Razg;}
    public void setr_T_ZISkorP_Razg(Float r_T_ZISkorP_Razg) {this.r_T_ZISkorP_Razg = r_T_ZISkorP_Razg;}
    
    public Float getr_T_ZISkorP_Torm() {return r_T_ZISkorP_Torm;}
    public void setr_T_ZISkorP_Torm(Float r_T_ZISkorP_Torm) {this.r_T_ZISkorP_Torm = r_T_ZISkorP_Torm;}
    
    public Float getr_T_ZISkorP_Fors() {return r_T_ZISkorP_Fors;}
    public void setr_T_ZISkorP_Fors(Float r_T_ZISkorP_Fors) {this.r_T_ZISkorP_Fors = r_T_ZISkorP_Fors;}
    
    public Float getZISkor() {return ZISkor;}
    public void setZISkor(Float ZISkor) {this.ZISkor = ZISkor;}
    
    public Float get_Ud() {return Ud;}
    public void set_Ud(Float Ud) {this.Ud = Ud;}
    
    public Float get_E() {return E;}
    public void set_E(Float E) {this.E = E;}
    
    public Float get_Idz() {return OuRegS_dop_kod;}
    public void set_Idz(Float OuRegS_dop_kod) {this.OuRegS_dop_kod = OuRegS_dop_kod;}   

    public Float get_IdzR() {return ZIDN;}                                              
    public void set_IdzR(Float ZIDN) {this.ZIDN = ZIDN;}
 // "Id"
    public void set_Id(Float Id_dop_kod ) {this.Id_dop_kod = Id_dop_kod;}

    public Float get_N() {return Skor;}                                                
    public void set_N(Float Skor ) {this.Skor = Skor;}

    public Float get_Lz() {return S_Alfa;}                                             
    public void set_Lz(Float S_Alfa ) {this.S_Alfa = S_Alfa;}

    public Float get_L() {return S_Alfa_Old;}                                           
    public void set_L(Float S_Alfa_Old ) {this.S_Alfa_Old = S_Alfa_Old;}

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("\n N#R:" + getZISkor());
        return sb.toString();
    }    
}


/*A class, of type JavaBean, that contains information about a variable for rendering on a graphical interface.*/
class Valuelist implements Comparable<Valuelist>{ 
	
	private Float id;
	private String name;
	private int id_element;
	private String id_svg;
	
	public Valuelist(String id_svg) {
		super();
		this.id_svg = id_svg;
	}
	
	public Valuelist(Float id) {
		super();
		this.id = id;
	}

	public Valuelist(Float id, String name) {
		super();
		this.id = id;
		this.name = name;
	}
	
	public Valuelist(int id_element, Float id, String name) {
		super();
		this.id_element = id_element;
		this.id = id;
		this.name = name;
	}
	
	public Valuelist(int id_element, Float id, String name, String id_svg) {
		super();
		this.id_element = id_element;
		this.id = id;
		this.name = name;
		this.id_svg = id_svg;
	}

	public Valuelist() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public String toString() {
		return "Valuelist [id_element=" + id_element + ", id=" + id + ", name=" + name +  ", id_svg=" + id_svg +"]";
	}
	
	public Float getId() {
		return id;
	}
	
	public void setId(Float id) {
		this.id = id;
	}
	

	@Override
	public int hashCode() {
		return Objects.hash(id, name);
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Valuelist other = (Valuelist) obj;
		return id == other.id && Objects.equals(name, other.name);
	}

	public String getName() {
		return name;
	}

	public void setIdSVG(String id_svg) {
		this.id_svg = id_svg;
	}
	
	@Override
	public int compareTo(Valuelist o) {  
		// TODO Auto-generated method stub
		if(name.length() > o.getName().length()) {         
					return 1;
		}else if(name.length() < o.getName().length()){     
	    	return -1;
	    }else {
	    	return 0;
	    }
	}
}

