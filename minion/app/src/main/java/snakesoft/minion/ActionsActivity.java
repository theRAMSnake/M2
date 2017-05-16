package snakesoft.minion;

import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;

import com.unnamed.b.atv.model.TreeNode;
import com.unnamed.b.atv.view.AndroidTreeView;

import actions.Actions;
import common.Common;

class TreeItemHolder extends TreeNode.BaseNodeViewHolder<Actions.ActionInfo> {
    public TreeItemHolder(Context context) {
        super(context);
    }

    public View createNodeView(TreeNode node, Actions.ActionInfo value)
    {
        final LayoutInflater inflater = LayoutInflater.from(context);
        final View view = inflater.inflate(R.layout.layout_profile_node, null, false);
        TextView tvValue = (TextView) view.findViewById(R.id.node_value);
        tvValue.setText(value != null ? value.getTitle() : "New...");

        return view;
    }
}

public class ActionsActivity extends AppCompatActivity implements TreeNode.TreeNodeLongClickListener
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_actions);

        TreeNode root = TreeNode.root();
        GlobalModel.getActionsModel().fillTreeRoot(root);

        AndroidTreeView tView = new AndroidTreeView(this, root);
        tView.setDefaultViewHolder(TreeItemHolder.class);
        tView.setDefaultAnimation(true);
        tView.setDefaultContainerStyle(R.style.TreeNodeStyleCustom);
        tView.setDefaultNodeLongClickListener(this);

        ((ConstraintLayout) findViewById(R.id.actionsLayout)).addView(tView.getView());
    }

    public boolean onLongClick(TreeNode node, Object value)
    {
        if(value != null)
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
        }
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
}
