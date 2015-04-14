(function() {
    Object.isBoolean = function(o) {
        return typeof o == 'boolean';
    }
    Object.isObject = function(o) {
        return typeof o == 'object';
    }
    Object.isSimpleObject = function(o) {
        return Object.isString(o) || Object.isNumber(o) 
                || Object.isBoolean(o) || o == null;
    }
    
    Array.prototype.insertAt=function(index,obj){ 
        return this.splice(index,0,obj); 
    } 
    Array.prototype.removeAt=function(index){ 
        return this.splice(index,1); 
    } 
    Array.prototype.remove=function(obj){ 
        var index=this.indexOf(obj); 
        if (index>=0){ 
            return this.removeAt(index); 
        } 
        return this;
    } 
    Array.prototype.extend=function(other){ 
        for (var i=0; i<other.length; i++){ 
            this.push(other[i]); 
        } 
        return this; 
    } 

    var C = {
        KEY_NAME: 'name',
        KEY_TYPE: 'type',
        KEY_DISPLAY_NAME: 'value',
        KEY_META: 'meta',        
        KEY_HEADER: 'header',
        KEY_DATA: 'data',
        KEY_OBJECT: 'object',
        KEY_TYPE_ID: 'id',
        OBJECT_ATTR_NAME_KEY: 'name',
        OBJECT_ATTR_NAME_LABEL: 'Name',
        OBJECT_ATTR_TYPE_KEY: 'type',
        OBJECT_ATTR_TYPE_LABEL: 'Type',
        OBJECT_ATTR_VALUE_KEY: 'value',
        OBJECT_ATTR_VALUE_LABEL: 'Value'
    };
    
    //表头
    var TableHeader = Class.create(Hash, {
        initialize: function($super, header) { 
            $super();
            if (!header) {
                this.none = true;
                return;
            }
            this.none = false;
                
            var cols = this;
            if (Object.isArray(header)) {
                for (var i = 0; i < header.length; ++i) { 
                    var item = header[i];
                    if (Object.isString(item)) {//['col1','col2']                            
                        var key = item;
                        cols.set(key, {name:item, displayName:item, index:i, type:null});
                    }
                    else if (item && item.hasOwnProperty(C.KEY_NAME)) {
                        var name = item[C.KEY_NAME];
                        var displayName = item[C.KEY_DISPLAY_NAME] || name;
                        var type = item[C.KEY_TYPE] || null;
                        var meta = item[C.KEY_META] || null;
                        cols.set(name, {name:name, displayName:displayName, index:i, type:type, meta:meta});
                    }
                    else {
                        var name = ""+i;
                        var displayName = item[C.KEY_DISPLAY_NAME] || name;
                        var type = item[C.KEY_TYPE] || null;
                        var meta = item[C.KEY_META] || null;
                        cols.set(name, {name:name, displayName:displayName, index:i, type:type, meta:meta});
                    }
                }
            }
            else if (Object.isObject(header)){//{col1:"col-1", col2:"col-2", ...}
                var keys = Object.keys(header);
                for (var i = 0; i < keys.length; ++i) { 
                    var name = keys[i];
                    var val = header[key];
                    cols.set(name, {name:name, displayName:val, index:i, type:null, meta:null});
                }
            }
            else {
                var key = header;
                cols.set(key, {name:key, displayName:key, index:0, type:null});
            }
        },
        getColNames: function() { 
            var cols = this;
            return cols.keys();
        },
        getColType: function(colName) { 
            if(this.isNone())
                return null;
            var cols = this;
            return cols.get(colName).type;
        },
        getColDisplayName: function(colName) { 
            if(this.isNone())
                return colName;
            var cols = this;
            return cols.get(colName).displayName;
        },
        getColIndex: function(colName) { 
            if(this.isNone())
                return 0;
            var cols = this;
            return cols.get(colName).index;
        },
        getColMeta: function(colName) { 
            if(this.isNone())
                return null;
            var cols = this;
            return cols.get(colName).meta;
        },        
        isNone: function() { 
            return this.none;
        }
    });
    //默认对象(属性)表头
    TableHeader.OBJECT_ATTRS_HEADER = (function() {
        var name = C.KEY_NAME;
        var displayName = C.KEY_DISPLAY_NAME;
        var col0 = new Object();
        col0[name] = C.OBJECT_ATTR_NAME_KEY;
        col0[displayName] = C.OBJECT_ATTR_NAME_LABEL;
        var col1 = new Object();
        col1[name] = C.OBJECT_ATTR_TYPE_KEY;
        col1[displayName] = C.OBJECT_ATTR_TYPE_LABEL;
        var col2 = new Object();
        col2[name] = C.OBJECT_ATTR_VALUE_KEY;
        col2[displayName] = C.OBJECT_ATTR_VALUE_LABEL;
        return [col0, col1, col2];
    })();
    
    //表格
    var BrpcTable = Class.create({
        initialize: function(parent, config) {
            this.parent = parent;
            
            this.config = config;
            this.header = new TableHeader(config.header);//[{name:"col1",value:"col-1"}, {name:"col2",value:"col-2"}, ...]
            this.data = config.data;//[{col1:1, col2:"value1"}, {col1:2, col2:"value2"}, ...]
            
            var that = this;
            
            //获取表头名称
            this.getColNames = config.getColNames ? config.getColNames : function(header) {
                if (header == that.header)
                    return header.getColNames();
                var row = header;
                header = new TableHeader(row);
                return header.getColNames();
            };
            //根据表头列名称获取列类型
            this.getColType = config.getColType ? config.getColType : function(col) {                
                return that.header.getColType(col);
            };
            //根据表头列名称获取列显示名称
            this.getColDisplayName = config.getColDisplayName ? config.getColDisplayName : function(col) {
                return that.header.getColDisplayName(col);
            };
            //根据表头列名称获取列元数据
            this.getColMeta = config.getColMeta ? config.getColMeta : function(col) {                
                return that.header.getColMeta(col);
            };
            //获取单元格数据的显示文本
            this.getLabel = config.getLabel ? config.getLabel : function(obj) {
                if (Object.isString(obj)) 
                    return obj;
                else if (Object.isNumber(obj) || Object.isBoolean(obj)) 
                    return obj.toString();
                else if (obj && obj.hasOwnProperty('label')) 
                    return obj.label;
                return '<unknown>';
            };
            //获取某行中的某列值
            this.getCellData = config.getCellData ? config.getCellData : function(row, col) {
                if (Object.isArray(row))
                    return row[col];
                else if (row && row.hasOwnProperty(col))
                    return row[col];
                else  if (row && Object.isSimpleObject(row) && col==row)
                    return row;
                return null;
            };
            //设置某行中的某列值
            this.setCellData = config.setCellData ? config.setCellData : function(row, col, val) {
                if (Object.isArray(row))
                    return row[col] = val;
                else if (row && row.hasOwnProperty(col))
                    return row[col] = val;
                else  if (row && Object.isSimpleObject(row) && col==row)
                    return row = val;
                return null;
            };
            //获取子表格数据
            this.getChildren = config.getChildren ? config.getChildren : function(obj) {
                if (Object.isArray(obj))
                    return {header:null, data:obj};
                else if (obj && Object.isObject(obj) && (attrs = Object.keys(obj)).length > 0) {
                    var header = TableHeader.OBJECT_ATTRS_HEADER;
                    
                    var col0 = C.OBJECT_ATTR_NAME_KEY;
                    var col1 = C.OBJECT_ATTR_TYPE_KEY;
                    var col2 = C.OBJECT_ATTR_VALUE_KEY;
                    
                    var data = new Array();
                    for (var i = 0; i < attrs.length; ++i) { 
                        var name = attrs[i];
                        var val = obj[name];
                        var type = typeof val;
                        
                        var row = new Object();
                        row[col0] = name;
                        row[col1] = type;
                        row[col2] = val;
                        data.push(row);
                    }
                    return {header:header, data:data};
                }
                else if (obj && obj.hasOwnProperty(C.KEY_DATA))
                    return {header:obj[C.KEY_HEADER], data:obj[C.KEY_DATA]};
                return {header:null, data:null};
            };
            
            // 将界面数据更新到模型
            this.onDataChanged = config.onDataChanged ? config.onDataChanged : function(cell, event) {
                var col = cell.retrieve(C.KEY_NAME);
                var data = cell.up().retrieve(C.KEY_DATA);
                var ele = event.element();
                
                var oldValue = data[col];
                var newValue = ele.value || ele.innerText;                        
                var updateObj = {table: that, rowIndex: BrpcTable.getRowIndex(cell.up()), cellName: col,
                                 oldValue: oldValue, newValue: newValue, event: event
                                };            
                if (config.onUpdateData && config.onUpdateData(updateObj) == false) {
                    if (ele.value == newValue)
                        ele.value = oldValue;
                    else if (ele.innerText == newValue)
                        ele.innerText = oldValue;
                }
                else
                    data[col] = newValue;
            };
             
            this.onCellClick = config.onCellClick ? config.onCellClick : function(){};
            this.onCellDblClick = config.onCellDblClick ? config.onCellDblClick : function(){};
             
            this.root = new Element('table');
            this.root.store(C.KEY_OBJECT, this);
            if (config.tableStyle)
                this.root.addClassName(config.tableStyle);
            this.parent.appendChild(this.root);
            
            this.loadHeader();
        },
        //  载入表头
        loadHeader: function(header) {              
            if (!Object.isUndefined(header) && header) 
                this.header = new TableHeader(header);
            if (!this.header || this.header.isNone())
                return;
            
            var config = this.config;        
            var createRow = this._createRow; 
            var createCell = this._createHeaderCell;
            
            var row = createRow();
            if (config.trStyle)
                row.addClassName(config.trStyle);
                        
            var cols = this.getColNames(this.header);
            for(var j = 0; j < cols.length; ++j) {
                var name = cols[j];
                var displayName = this.getColDisplayName(name);
                var cell = createCell();
                this._setCellProperty(cell, null, displayName, config.thStyle);
                
                row.appendChild(cell);
            }
            this.root.update('');
            this.root.appendChild(row);
        },
        // 载入数据
        load: function(data) {
            if (!Object.isUndefined(data) && data) 
                this.data = data;
            if (!this.data) 
                return;
            
            data = this.data;
            this.clear();
            this.data = data;
            this._loadData(this.data);
            this.root.store(C.KEY_DATA, this.data);
        },
        // 清空数据
        clear: function() {
            for(var i=this.root.children.length-1; i>0; i--)
                this.root.deleteRow(i);
            this.data = [];
        },
        // 增加一行数据
        appendRow: function(data) {
            if (Object.isUndefined(data) || !data) 
                return false;
            if (!this.data) 
                return false;
            
            this.data.push(data);//add one row
            data = [data];
            this._loadData(data);//add to ui
            return true;
        },
        // 删除一行数据
        removeRow: function(index) {
            if (Object.isUndefined(index) || index >= this.size())
                return false;
            if (index < 0)
                index = this.size() - 1;
            var data = this.data;
            data.removeAt(index);
            //this.load(data);//reload
            this.root.deleteRow(index);
            return true;
        },
        // 更新一行数据
        updateRow: function(index, data) {            
            if (Object.isUndefined(index) || index >= this.size())
                return false;
            this.data[index] = data;
            this.load();//@Todo: optimize
        },
        // 获取数据
        getData: function() {
            return this.data;
        },
        // 获取一行数据
        getRow: function(index) {
            if (!Object.isUndefined(index) && -1 < index && index < this.size())
                return this.data[index];
            return null;
        },
        // 获取行数
        size: function() {
            if (!this.data) 
                return 0;
            return this.data.length;
        },
        // 获取行索引
        getRowIndex: function(row) {
            if (!this.data) 
                return -1;
            for (var i=0; i<this.data.length; ++i) {
                if (this.data[i] == row)
                    return i;
            }
            return -1;
        },
        setWidth: function(w) {
            this.root.style.width = w;
        },
        setHeight: function(h) {            
            this.root.style.width = w;
        },
        _loadData: function(data) {
            var that = this;                      
            this._traverseData(this.root, data, function(parent, name, data, isCell) {
                var node = null;
                //td
                if (isCell) {
                    node = that._appendCell(name, data);
                }
                //tr
                else {
                    node = that._appendRow(name, data);
                }
                parent.appendChild(node);
                return node;
            });
        },
        _appendRow: function(name, data) {
            var config = this.config;
            var createRow = this._createRow; 
            
            var node = createRow();
            node.store(C.KEY_DATA, data);
            if (config.trStyle)
                node.addClassName(config.trStyle);
            return node;
        },
        _appendCell: function(name, data) {
            var me = this;
            var config = this.config;
            var createCell = this._createCell;
            
            var cell = createCell();
            me._setCellProperty(cell, name, me.getLabel(data), config.tdStyle);
            
            cell.observe('click', me.onCellClick);
            cell.observe('dblclick', me.onCellDblClick);
            cell.observe('mouseenter', function(e) {//mouseover会触发子元素的该事件
                if (config.overCellStyle) {
                    e.element().addClassName(config.overCellStyle);
                    //Event.stop(e);
                }
            });
            cell.observe('mouseleave', function(e) {//mouseout会触发子元素的该事件
                var ovrs = config.overCellStyle;
                if (ovrs && e.element().hasClassName(ovrs)) {
                    e.element().removeClassName(ovrs); 
                }  
            });
            cell.observe('change', function(e) {//onchange
                //var ele = e.element();
                //ele.value = "change:" + ele.value;
                me.onDataChanged(cell, e);
            });
            return cell;
        },
        // 创建行
        _createRow: function() {
            var row = new Element('tr');
            return row;
        },
        //创建单元格
        _createCell: function() {
            var cell = new Element('td'); 
            return cell;
        },
        //创建表头单元格
        _createHeaderCell: function() {
            var cell = new Element('th'); 
            return cell;
        },        
        // 遍历数据
        _traverseData: function(root, data, callback) {
            if (!data) 
                return;
            var rows;
            if (Object.isArray(data)) 
                rows = data;
            else
                rows = $A(data);
            
            //rows.each(function(row)
            //for (var i in rows)
            for (var i = 0; i < rows.length; ++i) { //for(var row in rows)
                var row = rows[i];
                var tr = callback(root, i, row, false);
                var cols = this.getColNames(!this.header.isNone() ? this.header : row);
                for(var j = 0; j < cols.length; ++j) {
                    var name = cols[j];                    
                    if (this.getColType(name) == C.KEY_TYPE_ID)
                        this.setCellData(row, name, this.getRowIndex(row)+1);
                    var col = this.getCellData(row, name);
                    var td = callback(tr, name, col, true);                    
                    //has children?
                    var children = this.getChildren(col);
                    if (children[C.KEY_DATA]) {//Todo: how to do?
                        var config = Object.clone(this.config);
                        config.data = null;
                        config.header = children[C.KEY_HEADER] || config.header;
                        td.update('');
                        var tb = new BrpcTable(td, config);
                        tb.load(children[C.KEY_DATA]);
                    }                    
                }
            }
        },
        // 设置节点的属性（数据,风格）
        _setCellProperty: function(node, name, text, normalClass) {            
            var type = name ? this.getColType(name) : null;
            //add input
            if (type && type != C.KEY_TYPE_ID) {  
                var that = this;
                // 创建控件
                function createInput(type, text) {
                    var inputItem = null;
                    if (type == 'select'){ 
                        inputItem = new Element(type);
                        
                        var selections = that.getColMeta(name);
                        var options = selections.split(',');
                        for (var i = 0; i < options.length; ++i) {
                            var kv = options[i].strip().split(':');
                            if (kv.length == 1)
                                kv.push(kv[0]);
                            inputItem.add(new Option(kv[0], kv[1]));
                        }
                        inputItem.setValue(text);
                    }
                    else {                    
                        inputItem = new Element('input'); 
                        inputItem.type = type; //'text','button'
                        inputItem.value = text;
                    }
                    inputItem.style.width = '98%';
                    //inputItem.style.display = '';
                    return inputItem;
                }
                var input = createInput(type, text);
                if (node && input) {
                    node.appendChild(input);
                }
            }
            //update content
            else {
                if (node && text) 
                    node.update(text);
            }
            //store col name into metadata
            node.store(C.KEY_NAME, name);
            //add style
            if (node && normalClass) {
                node.addClassName(normalClass); 
            }
        }
    });
    
    // 根据行内的子元素获取行 (静态方法)    
    BrpcTable.getRowByElement = function(ele) {
        while(ele && ele.tagName.toLowerCase() != "tr"){
            ele = ele.parentNode;
            if(ele.tagName.toLowerCase() == "table")
                return null;
        }
        if (ele.tagName.toLowerCase() == "tr")
            return ele;
        return null;
    }
    // 根据表格内的子元素获取表格 (静态方法)     
    BrpcTable.getTableByElement = function(ele) {
        while(ele && ele.tagName.toLowerCase() != "table"){
            ele = ele.parentNode;
        }
        if (ele.tagName.toLowerCase() == "table")
            return ele;
        return null;
    } 
    // 删除当前行(元素所在的行) (静态方法)
    BrpcTable.deleteRow = function(element) {
        var tr = BrpcTable.getRowByElement(element);
        if(tr != null) {
            var table = tr.parentNode;
            table.removeChild(tr);
            var data = table.retrieve(C.KEY_DATA);
            var row = tr.retrieve(C.KEY_DATA);
            if (data && row)
                return data.remove(row);
        }
        else
            throw new Error("the given object is not contained by the table"); 
        return null;
    }
    // 获取当前行号(静态方法)
    BrpcTable.getRowIndex = function(element){
        var trObj = BrpcTable.getRowByElement(element);//why trObj.rowIndex is -1?
        var trs = trObj.parentNode.children;
        var rowIndex = -1;
        for(var i= 0; i < trs.length; i++){
            var tr = trs[i];
            if(tr.children.length > 0 && tr.children[0].tagName.toLowerCase() == "td")
                rowIndex++;            
            if(trObj == tr)
                return rowIndex;
        }
        return -1;
    }
    
    window.deleteThisRow = BrpcTable.deleteRow;
    window.BrpcTable = BrpcTable;
})();