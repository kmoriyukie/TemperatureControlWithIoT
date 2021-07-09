class Map{
    engine;
    scale;
    // pos;
    // dim;
    walls;

    constructor(eng){
        this.engine = eng;

        this.addMote = function(posX,posY){
            return {pos: [posX,posY],dim: [50,50], draw(engine,pos){
                // console.log("My pos", this.pos);
                // console.log("F pos", pos);
                // pos[0] -= this.dim[0]/2;
                // pos[1] -= this.dim[1]/2;
                // pos = [0,0];
                engine.push();        
                // engine.translate(pos[0]/1.0+this.pos[0], pos[1]/1.0+this.pos[1]);
                engine.strokeWeight(1);
                engine.ellipse(pos[0]+this.pos[0], pos[1]+this.pos[1], this.dim[0], this.dim[0]);
                // engine.quad(this.pos[0],this.pos[1],this.pos[0],this.pos[1]+this.dim[1],this.pos[0]+this.dim[0],this.pos[1]+this.dim[1],this.pos[0]+this.dim[0],this.pos[1]);
                engine.pop();
            }};
        }

        this.motes = [this.addMote(0,0)];//,this.addMote(0,0),this.addMote(0,0),this.addMote(0,0)];//this.addMote(500,200),this.addMote(250,400),this.addMote(0,200)];

        this.walls = {pos: [20,20],dim: [500,400], motes: this.motes, draw(engine){
            engine.push();        
            // engine.translate(this.pos[0], this.pos[1]);
            engine.strokeWeight(4);
            engine.rect(this.pos[0],this.pos[1],this.dim[0],this.dim[1]);
            this.motes.forEach(element => {
                element.draw(engine,this.pos);
            });
            engine.pop();
        }};

        this.mote = this.addMote();

        this.scale = 1;

    }
    // setup(){

    // }
    draw(){
        engine.background(255);
        engine.push();
        
        // engine.translate(this.pos[0], this.pos[1], 0);
        engine.scale(this.scale);
        // this.draw_walls(this.engine);
        this.walls.draw(this.engine);
        // this.walls.draw(this.engine);
        // engine.strokeWeight(4);
        // engine.quad(this.pos[0],this.pos[1],this.pos[0],this.pos[1]+this.dim[1],this.pos[0]+this.dim[0],this.pos[1]+this.dim[1],this.pos[0]+this.dim[0],this.pos[1]);
        engine.pop();

        // noStroke();

    }

    // draw_walls(engine){
    //     engine.push();        
    //     engine.translate(this.pos[0], this.pos[1], -1);
    //     engine.strokeWeight(4);
    //     engine.quad(this.pos[0],this.pos[1],this.pos[0],this.pos[1]+this.dim[1],this.pos[0]+this.dim[0],this.pos[1]+this.dim[1],this.pos[0]+this.dim[0],this.pos[1]);
    //     engine.pop();
    // }
}
