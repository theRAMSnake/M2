package snakesoft.minion;

import android.content.Intent;
import android.database.DataSetObserver;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListAdapter;
import android.widget.ListView;

import java.util.List;
import java.util.Vector;

import inbox.Inbox;

public class InboxActivity extends AppCompatActivity implements AdapterView.OnItemClickListener
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_inbox);

        Vector<String> itemsAsString = new Vector<>();
        if(GlobalModel.getInboxModel().getItems() != null)
        {
            for(Inbox.InboxItemInfo x : GlobalModel.getInboxModel().getItems().getItemsList())
            {
                itemsAsString.add(x.getText());
            }
        }

        ArrayAdapter<String> itemsAdapter =
                new ArrayAdapter<>(this,
                        android.R.layout.simple_list_item_1,
                        itemsAsString);

        ListView listView = (ListView) findViewById(R.id.lvMain);
        listView.setAdapter(itemsAdapter);

        listView.setOnItemClickListener(this);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == 1)
        {
            recreate();
        }
    }

    public void onItemClick(AdapterView<?> parent, View view, int position, long id)
    {
        Intent myIntent = new Intent(InboxActivity.this, InboxItemViewActivity.class);
        myIntent.putExtra("Text", GlobalModel.getInboxModel().getItems().getItemsList().get(position).getText());
        myIntent.putExtra("Id", GlobalModel.getInboxModel().getItems().getItemsList().get(position).getId().getGuid());
        myIntent.putExtra("IsNewItem", false);
        InboxActivity.this.startActivityForResult(myIntent, 1);
    }
}