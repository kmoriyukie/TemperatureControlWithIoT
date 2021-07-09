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

let sketch = function(engine){
    self.engine = engine;
    engine.setup = function(){
        // my_setup(engine);
    };
    engine.draw = function(){
        // my_draw(engine);
    }
}
