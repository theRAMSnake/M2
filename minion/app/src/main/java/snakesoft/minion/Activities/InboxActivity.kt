package snakesoft.minion.Activities

import android.content.Intent
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.ListView

import java.util.Vector

import inbox.Inbox
import snakesoft.minion.Models.GlobalModel
import snakesoft.minion.R

class InboxActivity : AppCompatActivity(), AdapterView.OnItemClickListener {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_inbox)

        val itemsAsString = Vector<String>()
        if (GlobalModel.InboxModel.items != null) {
            itemsAsString.add("New...")
            for (x in GlobalModel.InboxModel.items) {
                itemsAsString.add(x.getText())
            }
        }

        val itemsAdapter = ArrayAdapter(this,
                android.R.layout.simple_list_item_1,
                itemsAsString)

        val listView = findViewById(R.id.lvMain) as ListView
        listView.adapter = itemsAdapter

        listView.onItemClickListener = this
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent) {
        super.onActivityResult(requestCode, resultCode, data)

        if (requestCode == 1) {
            recreate()
        }
    }

    override fun onItemClick(parent: AdapterView<*>, view: View, position: Int, id: Long) {
        if (position != 0) {
            val myIntent = Intent(this@InboxActivity, InboxItemViewActivity::class.java)
            myIntent.putExtra("Text", GlobalModel.InboxModel.items.get(position - 1).getText())
            myIntent.putExtra("Id", GlobalModel.InboxModel.items.get(position - 1).getId().getGuid())
            myIntent.putExtra("IsNewItem", false)
            this@InboxActivity.startActivityForResult(myIntent, 1)
        } else {
            val myIntent = Intent(this@InboxActivity, InboxItemViewActivity::class.java)
            myIntent.putExtra("IsNewItem", true)
            myIntent.putExtra("Id", GlobalModel.InboxModel.newId)
            this@InboxActivity.startActivityForResult(myIntent, 1)
        }
    }
}