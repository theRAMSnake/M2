package snakesoft.minion;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;

import com.unnamed.b.atv.model.TreeNode;

import actions.Actions;

public class TreeItemHolder extends TreeNode.BaseNodeViewHolder<Actions.ActionInfo> {
    public TreeItemHolder(Context context)
    {
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
