function my_setup(engine) {
    engine.createCanvas(500, 400);
    engine.background(0);
}
    
function my_draw(engine) {
    if(engine.mouseIsPressed) {
    } else {
    }
}

var sketch = function(engine){
    self.engine = engine;
    engine.setup = function(){
    };
    engine.draw = function(){
    }
}

class myTransf{
    mat;
    constructor(){
        this.mat = math.identity(3);
    }
    static translationMatrix(pos){
        let ret = math.identity(3);
        ret._data[0][2] = pos[0];
        ret._data[1][2] = pos[1];
        return ret;
    }
    static rotationMatrix(rot){
        let ret = math.identity(3);
        ret._data[0][0] = math.cos(rot);
        ret._data[1][0] = math.sin(rot);
        ret._data[0][1] = -math.sin(rot);
        ret._data[1][1] = math.cos(rot);
        return ret;
    }
    static scaleMatrix(scale){
        let ret = math.identity(3);
        ret._data[0][0] = scale;
        ret._data[1][1] = scale;
        return scale;
    }
}

