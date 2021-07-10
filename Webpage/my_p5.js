function my_setup(engine) {
    engine.createCanvas(500, 400);
    engine.background(0);
}
    
function my_draw(engine) {
    if(engine.mouseIsPressed) {
        // engine.fill(0);
    } else {
        // engine.fill(255);
    }
}

var sketch = function(engine){
    self.engine = engine;
    engine.setup = function(){
        // my_setup(engine);
    };
    engine.draw = function(){
        // my_draw(engine);
    }
}

// var GUI_SELECTED = "none";

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
    // translate(pos){
    //     this.mat = math.multiply(this.mat,this.translationMatrix(pos));
    //     return this.mat;
    // }
    // rotate(rot){
    //     this.mat = math.multiply(this.mat,this.rotationMatrix(rot));
    //     return this.mat;
    // }
    // scale(sc){
    //     this.mat = math.multiply(this.mat,this.scaleMatrix(sc));
    //     return this.mat;
    // }
    // getMatObj(){
    //     return this.mat;
    // }
    // getMat(){
    //     return this.mat._data;
    // }
    // setmat(mat){
    //     this.mat = mat;
    // }
}

// let a = new Transf();
// let b = new Transf();

// console.table(a.getMat());
// a.translate([30,50]);
// b.rotate(math.PI/4);
// console.table(b.getMat());
