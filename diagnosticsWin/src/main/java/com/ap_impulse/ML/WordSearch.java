package com.ap_impulse.ML;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.regex.Pattern;

import org.deeplearning4j.models.paragraphvectors.ParagraphVectors;
import org.deeplearning4j.models.word2vec.VocabWord;
import org.deeplearning4j.models.word2vec.Word2Vec;
import org.deeplearning4j.models.word2vec.wordstore.inmemory.AbstractCache;
import org.deeplearning4j.text.documentiterator.LabelsSource;
import org.deeplearning4j.text.sentenceiterator.BasicLineIterator;
import org.deeplearning4j.text.sentenceiterator.SentenceIterator;
import org.deeplearning4j.text.sentenceiterator.SentencePreProcessor;
import org.deeplearning4j.text.tokenization.tokenizer.preprocessor.CommonPreprocessor;
import org.deeplearning4j.text.tokenization.tokenizerfactory.DefaultTokenizerFactory;
import org.deeplearning4j.text.tokenization.tokenizerfactory.TokenizerFactory;
import org.nd4j.common.io.ClassPathResource;
import org.nd4j.linalg.ops.transforms.Transforms;
import org.nd4j.shade.guava.base.Splitter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class WordSearch {
	private static Logger log = LoggerFactory.getLogger(WordSearch.class);
	public static String dataLocalPath;
	public static String exceptionMessage;	
	public static List<String> lst_buffer = new ArrayList<>();
	public static List<String> lst = new ArrayList<>();	
	public static List<String> lst_sentence = new ArrayList<>();	 // буфер для перекладання строк, для виключення дубляжа 
	public static List<String> lst_answer = new ArrayList<>();	
	private static ParagraphVectors vec;
	private static TokenizerFactory t;
	private static SentenceIterator iter;
	
	public WordSearch() {
		System.out.println("--- Start WordSearch constructor and training with AnswerController---");
		try {
			training();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	// downloading and training
	public void training() throws IOException {
		// Завантаження ресурсу
    	String filePath = new ClassPathResource("словникML.txt").getFile().getAbsolutePath();
    	//System.out.println(filePath +"------------------------");
    	//C:\eclipse-workspace\dl4j_Doc2Vec\target\classes\словникML.txt------------------------
    	log.info("Load & Vectorize Sentences....");
    	// Strip white space before and after for each line
    	iter = new BasicLineIterator(filePath); // Інкапсуляція тексту построково
    	AbstractCache<VocabWord> cache = new AbstractCache<>();
    	
    	// Викликається вісля Building model....
        iter.setPreProcessor(new SentencePreProcessor() {
    	    @Override
    	    public String preProcess(String sentence) {
    	    	//String utf8Strin = new String(sentence.getBytes("UTF-8"), "Cp1251");
    	    	if (sentence!= null) lst_buffer.add(sentence.toLowerCase());
    	        return sentence.toLowerCase();
    	    }
    	});
        
        // ------- Мітки ---------------------------
        LabelsSource source = new LabelsSource("DOC_");
    
    	// Split on white spaces in the line to get words
    	t = new DefaultTokenizerFactory();
    	t.setTokenPreProcessor(new CommonPreprocessor() {
    	 @Override
    	    public String preProcess(String token) {
    		    Pattern punctPattern = Pattern.compile("[\\.:,\"\'\\(\\)\\[\\]|/?!;]+");
		  		return punctPattern.matcher(token).replaceAll("").toLowerCase();
		  		//наразі коментую - прописав соій вираз, щоб залишити цифри було compile("[\\d\\.:,\"\'\\(\\)\\[\\]|/?!;]+")
    	        //return StringCleaning.stripPunct(token).toLowerCase();
    	    }
    	});
    	 
    	log.info("Building model....");
    	 
    	vec = new ParagraphVectors.Builder()
    	        .minWordFrequency(1)
	            .iterations(5)
    	        .epochs(1)
    	        .layerSize(100)
    	        .learningRate(0.025)
    	        .labelsSource(source)
    	        .windowSize(5)
    	        .iterate(iter)
    	        .trainWordVectors(false)
    	        .vocabCache(cache)
    	        .tokenizerFactory(t)
    	        .sampling(0)
    	        .build();

    	vec.fit();

    	lst = vec.getLabelsSource().getLabels(); // повернення міток
  	  
    	for (String doc_number: lst_buffer ) {
            if (lst_sentence.size() < lst.size()) lst_sentence.add(doc_number);
    	}
    	lst_buffer.clear();
    	//System.out.println("lst_sentence " + lst_sentence);
    	System.out.println("Кількість міток (рядків) в ловнику: "+lst.size());
	}
	
	    	
	//comparison and search
	public String SearchComparision( String i_news ) throws IOException {
	    //ВВеденя речення для порівняння
	    //i_news = "Тестування аналогових входів";
		String string_answer = null;
	    List<String> news = new ArrayList<>(1); 
	    news.add(i_news);
	    System.out.println(news.size()); 							// 1 - ( кількість строк )
	    	        
	    for (String s : news) { 									// слова або речення
	        List<String> tokens = t.create(s).getTokens(); 			// Повертає список усіх токенів
	    	System.out.println(tokens);  							// [тестування, аналогових, входів]
	    	List<String> tokensFiltered = new ArrayList<>();
	    	for (String t1 : tokens) {  							// токени (слова)
	    	   if (vec.hasWord(t1)) tokensFiltered.add(t1);         // Повертає true, якщо модель містить це слово у словнику і додаємо слово
	    	}
	    	System.out.println(tokensFiltered); 					// [тестування, аналогових, входів]
	    	i_news = "";
	    	for (String t1 : tokensFiltered) {
	    	   i_news = i_news + " "+ t1;
	    	}
	    	System.out.println(i_news);  							// тестування аналогових входів
	    }
	    if ( i_news == "" ) return string_answer = "В словнику відсутні такі дані!";
	    
	    //for (String doc_number: lst ) {
	    //    System.out.println("Similarity Score between: "+i_news+" --vs-- "+ doc_number +":==>"+ cosineSimForSentence(vec, i_news, doc_number));  
	    //}
	    
	    
	    float key = 0;
        Map<Float,String> MAP1 = new TreeMap<>();  
        // Порівняння двума способами. Порівняння тексту з текстом
   	    // Порівняння двума способами. Порівняння тексту з текстом
	    for (String doc_number: lst_sentence ) {
          	//System.out.println(i_news + " vs " + doc_number);
           	//System.out.println(doc_number);
	    	
	    	//key = (float) cosineSimForSentence(vec, i_news, doc_number);
	    	
	    	Collection<String> label1 = Splitter.on(' ').splitToList(i_news);
	      	Collection<String> label2 = Splitter.on(' ').splitToList(doc_number);
	      	//try{
	      	//	key =  (float) Transforms.cosineSim(vec.getWordVectorsMean(label1), vec.getWordVectorsMean(label2));
	       	//}catch(Exception e){
	       	//	exceptionMessage = e.getMessage();
	       	//}
	       	key =  (float) Transforms.cosineSim(vec.getWordVectorsMean(label1), vec.getWordVectorsMean(label2));
	    	
	        //System.out.println("Similarity Score between: "+i_news+" --vs-- "+ doc_number +":==>"+ key);
	        MAP1.put(key, doc_number);
   	       	//System.out.println("Similarity Score between: "+i_news+" --vs-- "+ doc_number +":==>"+ cosineSimForSentence(vec, i_news, doc_number));
   	       	//INDArray inferredVectorA1 = vec.inferVector(i_news);
    	    //INDArray inferredVectorB1 = vec.inferVector(doc_number);
    	    //inferVector метод обчислює вихідний вектор для заданого тексту з параметрами за замовчуванням для швидкості навчання та ітерацій
    	    //System.out.println("Cosine similarity A/B:" + Transforms.cosineSim(inferredVectorA1, inferredVectorB1));
   	    }	     
	 
    	System.out.println("MAP1 "+ MAP1);
    	System.out.println("MAP1 "+ MAP1.size());
    	
    	List<String> lst_answer1 = new ArrayList<>();
    	for (Map.Entry entry: MAP1.entrySet()) {
    		   System.out.println(entry);
    		   lst_answer1.add( entry.getKey()+ " " + entry.getValue());
    		}
    	
    	for(int i = lst_answer1.size(); i > lst_answer1.size()-5 ; i-- ){
    		lst_answer.add(lst_answer1.get(i-1));
    		//string_answer = string_answer +" " + (lst_answer1.get(i-1));
    	}
    	System.out.println(lst_answer);
    	return string_answer = lst_answer.get(0);
	    
	}
	
	// Метод порівняння https://stackoverflow.com/questions/48636407/dl4j-how-to-calculate-cosine-similarity-between-indarray-obtained-from-getwordv
	// Обчислення подібності (косинус подібності) між обома реченнями, використовуючи їхнє середнє значення
    public double cosineSimForSentence(Word2Vec vector, String sentence1, String sentence2){
      	Collection<String> label1 = Splitter.on(' ').splitToList(sentence1);
      	Collection<String> label2 = Splitter.on(' ').splitToList(sentence2);
      	try{
      		return Transforms.cosineSim(vector.getWordVectorsMean(label1), vector.getWordVectorsMean(label2));
       	}catch(Exception e){
       		exceptionMessage = e.getMessage();
       	}
       	return Transforms.cosineSim(vector.getWordVectorsMean(label1), vector.getWordVectorsMean(label2));
       	//getWordVectorsMean метод повертає вектор середнього значення, побудований із переданих слів/міток
    }
}
