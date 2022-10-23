package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Screen;
import com.badlogic.gdx.graphics.OrthographicCamera;
import com.badlogic.gdx.scenes.scene2d.InputEvent;
import com.badlogic.gdx.scenes.scene2d.Stage;
import com.badlogic.gdx.scenes.scene2d.ui.Skin;
import com.badlogic.gdx.scenes.scene2d.ui.Table;
import com.badlogic.gdx.scenes.scene2d.ui.TextButton;
import com.badlogic.gdx.scenes.scene2d.utils.ClickListener;
import com.badlogic.gdx.utils.ScreenUtils;
import com.badlogic.gdx.utils.viewport.ExtendViewport;
import com.badlogic.gdx.utils.viewport.Viewport;

public class PauseScreen implements Screen {

    private MainGame game;
    private GameScreen gameScreen;

    private OrthographicCamera camera;

    private Stage stage;
    private Viewport viewport;
    private Table table;
    private Skin skin;


    public PauseScreen(MainGame game, GameScreen gameScreen) {
        this.game = game;
        this.gameScreen = gameScreen;

        camera = new OrthographicCamera(Gdx.graphics.getWidth(), Gdx.graphics.getHeight());
    }

    @Override
    public void show() {

        stage = new Stage();
        viewport = new ExtendViewport(Gdx.graphics.getWidth(), Gdx.graphics.getHeight());

        skin = new Skin(Gdx.files.internal("pixthulhu-ui.json"));

        table = new Table();
        table.setFillParent(true);

        stage.addActor(table);

        addButton("Resume").addListener(new ClickListener(){
            @Override
            public void clicked(InputEvent event, float x, float y) {
                gameScreen.resume();
            }
        });
        addButton("Quit").addListener(new ClickListener(){
            @Override
            public void clicked(InputEvent event, float x, float y) {
                Gdx.app.exit();
            }
        });

        Gdx.input.setInputProcessor(stage);

    }

    private TextButton addButton(String name){
        TextButton button = new TextButton(name, skin);
        table.add(button);
        table.row();

        return button;
    }


    @Override
    public void render(float delta) {
        update();
        draw();
    }

    public void update(){
        ScreenUtils.clear(0, 0, 0,1);
        camera.update();
        game.batch.setProjectionMatrix(camera.combined);
    }

    public void draw(){
        stage.act();
        stage.draw();
    }

    @Override
    public void resize(int width, int height) {

    }

    @Override
    public void pause() {

    }

    @Override
    public void resume() {

    }

    @Override
    public void hide() {

    }

    @Override
    public void dispose() {

    }
}
