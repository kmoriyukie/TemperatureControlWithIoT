class ConfigCanvas extends p5{
    map;
    dim;
    parent;
    sc;
    constructor(){
        super(sketch, window.document.getElementById("ConfigCanvas"));
        this.setup = this.m_setup;
        this.draw = this.m_draw;
        this.parent = [];
        this.parent.push(new Map(this));
        this.parent[0].mode = "config";
        this.mouseClicked = this.m_mouseclick;
        this.MapTransf = math.identity(3);
        this.visible = true;
        this.sc = 1;
        this.onPreload();
    }
    onPreload(){
        this.font = this.loadFont(
            "assets/Staatliches-Regular.ttf"
          );
        
        this.parent.forEach(element => {
            element.onPreload();
        });
    }
    m_setup(){
        this.dim = [900, 600];
        this.createCanvas(this.dim[0], this.dim[1]);
        this.background('rgb(255, 247, 235)');
        this.textFont(this.font);
    }

    m_draw(){
        this.background('rgb(255, 247, 235)');

        this.push();
        this.parent.forEach(element => {
            element.draw();
        });
        this.pop();
    }
    m_mouseclick(){
        if(!this.visible) return;
        this.detect_hitbox([this.mouseX,this.mouseY]);
    } 
    detect_hitbox(point){
        if(this.visible){
            if(point[0] >= 0 && point[0] <= (this.dim[0]*this.sc)){
                if(point[1] >= 0 && point[1] <= (this.dim[1]*this.sc)){
                    this.default_dHit(point);
                    return true;
                }
            }
        }
        return false;
    }
    onClick(){
    }
    default_dHit(point){
        this.onClick();
        this.parent.forEach(element => {
            switch(element.type){
                case "Map":
                    let newP = math.multiply(this.MapTransf, math.matrix([point[0],point[1],1]));
                    element.detect_hitbox([newP._data[0],newP._data[1]]);
                break;
            }
        });
    }
}

var CONF_CANV = new ConfigCanvas();