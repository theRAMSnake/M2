package snakesoft.minion.Activities

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.widget.TextView

import com.unnamed.b.atv.model.TreeNode

import snakesoft.minion.R

class TreeItemHolder(context: Context) : TreeNode.BaseNodeViewHolder<String>(context) {

    override fun createNodeView(node: TreeNode, value: String): View {
        val inflater = LayoutInflater.from(context)
        val view = inflater.inflate(R.layout.layout_profile_node, null, false)
        val tvValue = view.findViewById(R.id.node_value) as TextView
        tvValue.text = value

        return view
    }
}
