(function() {
    var C = {
        NODE_STATE_EXPAND: '[+]',
        NODE_STATE_COLLAPSE: '[-]',
        NODE_STATE_LEAF: '',
        
        LABEL_NULL: '<unknown>'
    }
    
    var Tree = Class.create({
        initialize: function(element, config) {
            this.element = element;
             
            this.data = config.data;
            this.disableCollapse = config.disableCollapse;
            this.selectedClass = config.selectedStype;
            this.normalClass = config.normalStype;
             
            this.getChildren = config.getChildren ? config.getChildren : function(o) {
                return Object.isArray(o.children) ? o.children : [];
            };
            this.getLabel = config.getLabel ? config.getLabel : function(o) {
                return Object.isUndefined(o.label) ? C.LABEL_NULL : o.label;
            };
             
            this.onNodeClick = config.onNodeClick ? config.onNodeClick : function(){};
            this.onNodeDblClick = config.onNodeDblClick ? config.onNodeDblClick : function(){};
             
            this.root = new Element('ul');
            this.element.appendChild(this.root);
        },
        // 载入数据，创建树。分离此步骤,更灵活地把握创建时刻。
        load: function(data) {
            if (!Object.isUndefined(data) && data) 
                this.data = data;
            if (!this.data) 
                return;
            
            this.root.update('');
            this.root.appendChild(this._load(this.data));
        },
        // 更新数据
        updateChildren: function(node, data) {
            if (!this.data || !node) 
                return;
            
            var root = this._load(data);
            root = root.down('ul');
            if (root) {
                node.appendChild(root);
                var anchor = node.childElements()[0];
                this._updateCollapseState(anchor);
            }
        },
        _load: function(data) {
            var _createNode = this._createNode;
            var _setNode = this._setNode;
            var getLabel = this.getLabel;
            var normalClass = this.normalClass;
             
            var rootLiNode = this._traverseData(data, 0, function(data, level, nodes) {
                var li = _createNode();
                _setNode(li, level, C.NODE_STATE_LEAF, getLabel(data), normalClass);
                li.store('object', data);
                var length = nodes.length;
                if (length > 0) {
                    var ul = new Element('ul');
                    for (var i = 0; i < length; ++i) {
                        ul.appendChild(nodes[i]);
                    }
                    li.appendChild(ul);
                }
                return li;
            });
            
            var me = this;
            this._traverseNode(rootLiNode, 0, function(children, results) {                
                if (!me.disableCollapse) {
                    var anchor = children[0];
                    me._updateCollapseState(anchor);
                }

                var label = children[1];
                label.observe('click', me.onNodeClick);
                label.observe('dblclick', me.onNodeDblClick);
                label.observe('click', function(e) {
                    if (me.selectedClass) {
                        if (me.selectedNode) 
                            me.selectedNode.removeClassName(me.selectedClass);
                        me.selectedNode = e.element();
                        me.selectedNode.addClassName(me.selectedClass);
                    }
                });                    
            });
            
            return rootLiNode;
        },
        // 创建节点，虽然这里封装了，但其他地方深入了节点的内部结构，这块没有做好设计。
        _createNode: function() {
            var li = new Element('li');
            li.appendChild(new Element('a', {href: 'javascript:void(0)'}));
            li.appendChild(new Element('span'));
            return li;
        },
        // 设置节点的属性（数据）
        _setNode: function(node, level, button, label, normalClass) {
            var children = node.childElements();
            var nodeButton = children[0];
            var nodeLabel = children[1];
            
            if (!Object.isUndefined(button)) 
                nodeButton.update(button);
            if (!Object.isUndefined(label))
                nodeLabel.update(label);
            
            if (!Object.isUndefined(normalClass) && normalClass) {
                nodeButton.addClassName(normalClass);  
                nodeLabel.addClassName(normalClass);                
            }
        },
        // 设置节点的状态
        _updateCollapseState: function(anchor) {        
            var me = this;
            function collapse(e) {
                var a = e.element();
                var next = a.next(1);
                if (next) {
                    var state = (a.innerHTML == C.NODE_STATE_COLLAPSE) ? C.NODE_STATE_EXPAND : C.NODE_STATE_COLLAPSE;
                    a.update(state);
                    next.toggle();
                }
            }
            
            var next = anchor.next(1);
            if (next) {
                anchor.update(C.NODE_STATE_COLLAPSE);
                anchor.observe('click', collapse);                        
            }
            else {
                //anchor.update(C.NODE_STATE_LEAF);
            }
        },
        // 后序遍历数据，适合某一个数据及其子树数据的综合计算。data：数据对象；level：树深；callback：回调函数，格式如下                
        // callback(data, level, results)：返回回调的结果，通常是这个节点及其子树的综合值；data：数据对象；level：树深；results：子树的计算结果数组。
        _traverseData: function(data, level, callback) {
            if (!data) return;
             
            var children = this.getChildren(data);
            var n = children.length;
            var results = new Array(n);
            for (var i = 0; i < n; ++i) {
                results[i] = this._traverseData(children[i], level + 1, callback);
            }
             
            return callback(data, level, results);
        },
        // 后序遍历结构，适合某一个节点及其子树节点的综合计算。data：数据对象；level：树深；callback：回调函数，格式如下                
        // callback(children, results)：返回回调的结果，通常是这个节点及其子树的综合值；results：数据对象；results：子树的计算结果数组。（这里忘了level……）
        _traverseNode: function(liNode, level, callback) {
            var children = liNode.childElements();
            var length = children.length;
            var results;
            if (length > 2) {
                var ul = children[2];
                var lis = ul.childElements();
                var n = lis.length;
                results = new Array(n);
                for (var i = 0; i < n; ++i) {
                    results[i] = this._traverseNode(lis[i], level + 1, callback);
                }
            } else {
                results = [];
            }
            return callback(children, results);
        }
    });
     
    window.Tree = Tree;
})();