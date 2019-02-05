package snakesoft.minion.Activities

import android.content.Intent
import android.os.Bundle
import android.support.constraint.ConstraintLayout
import android.support.v7.app.AppCompatActivity

import com.unnamed.b.atv.model.TreeNode
import com.unnamed.b.atv.view.AndroidTreeView

import snakesoft.minion.R

class ActionsActivity : AppCompatActivity(), TreeNode.TreeNodeLongClickListener {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_actions)

        /*final String mode = getIntent().getStringExtra("Mode");

        TreeNode root = TreeNode.root();
        GlobalModel.getActionsModel().fillTreeRoot(root, mode.equals("Normal")
                ? ActionsTreeType.Normal : ActionsTreeType.Backlog);

        AndroidTreeView tView = new AndroidTreeView(this, root);
        tView.setDefaultViewHolder(TreeItemHolder.class);
        tView.setDefaultAnimation(true);
        tView.setDefaultContainerStyle(R.style.TreeNodeStyleCustom);
        tView.setDefaultNodeLongClickListener(this);

        ((ConstraintLayout) findViewById(R.id.actionsLayout)).addView(tView.getView());*/
    }

    override fun onLongClick(node: TreeNode, value: Any): Boolean {
        /*if(value != null)
        {
            Actions.ActionInfo item = (Actions.ActionInfo)value;

            Intent myIntent = new Intent(ActionsActivity.this, ActionsItemViewActivity.class);
            myIntent.putExtra("Title", item.getTitle());
            myIntent.putExtra("Desc", item.getDescription());
            myIntent.putExtra("Id", item.getId().getGuid());
            myIntent.putExtra("ParentId", item.getParentId().getGuid());
            myIntent.putExtra("Type", item.getType().ordinal());
            myIntent.putExtra("IsNewItem", false);

            ActionsActivity.this.startActivityForResult(myIntent, 1);

            return true;
        }
        else
        {
            Actions.ActionInfo item = (Actions.ActionInfo)value;

            final Intent myIntent = new Intent(ActionsActivity.this, ActionsItemViewActivity.class);

            Object parentValue = node.getParent().getValue();
            if (parentValue != null)
            {
                myIntent.putExtra("ParentId", ((Actions.ActionInfo)parentValue).getId().getGuid());
            }

            myIntent.putExtra("Id", GlobalModel.getActionsModel().genId());
            myIntent.putExtra("IsNewItem", true);

            ActionsActivity.this.startActivityForResult(myIntent, 1);
            return true;
        }*/
        return true
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent) {
        super.onActivityResult(requestCode, resultCode, data)

        if (requestCode == 1) {
            recreate()
        }
    }
}
