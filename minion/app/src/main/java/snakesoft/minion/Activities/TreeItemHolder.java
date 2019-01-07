package snakesoft.minion.Activities;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;

import com.unnamed.b.atv.model.TreeNode;

import snakesoft.minion.R;

public class TreeItemHolder extends TreeNode.BaseNodeViewHolder<String> {
    public TreeItemHolder(Context context)
    {
        super(context);
    }

    public View createNodeView(TreeNode node, String value)
    {
        final LayoutInflater inflater = LayoutInflater.from(context);
        final View view = inflater.inflate(R.layout.layout_profile_node, null, false);
        TextView tvValue = (TextView) view.findViewById(R.id.node_value);
        tvValue.setText(value);

        return view;
    }
}
