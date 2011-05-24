package com.fryit.androidtelemeter;

import java.util.ArrayList;
import java.util.HashMap;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

public class ListMenuActivity extends Activity {

	private ListView ListMenu;
	 
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.menulist);
        
        ListMenu = (ListView) findViewById(R.id.menuList);
 
        ArrayList<HashMap<String, String>> listItem = new ArrayList<HashMap<String, String>>(); //ArrayList to fill the Menu

        HashMap<String, String> map;
 
        //ITEM1: Telemeter
        map = new HashMap<String, String>();
        map.put("titre", "Telemeter");
        map.put("description", "Application to measure objects with your camera");
        map.put("img", String.valueOf(R.drawable.telemetericon));
        listItem.add(map);
 
        //Création d'un SimpleAdapter qui se chargera de mettre les items présent dans notre list (listItem) dans la vue affichageitem
        SimpleAdapter mSchedule = new SimpleAdapter (this.getBaseContext(), listItem, R.layout.menuitems,
               new String[] {"img", "titre", "description"}, new int[] {R.id.img, R.id.title, R.id.description});
 
        //On attribut à notre listView l'adapter que l'on vient de créer
        ListMenu.setAdapter(mSchedule);
 
        //Enfin on met un écouteur d'évènement sur notre listView
        ListMenu.setOnItemClickListener(new OnItemClickListener() {
			@Override
        	@SuppressWarnings("unchecked")
         	public void onItemClick(AdapterView<?> a, View v, int position, long id) {
				switch (position) {
				case 0:
					Intent intent = new Intent(ListMenuActivity.this, TelemeterActivity.class);
					startActivity(intent);
					break;
				default:
					
					break;
				}
        	}
         });
 
    }
}