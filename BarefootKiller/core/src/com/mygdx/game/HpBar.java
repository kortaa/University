package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Color;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.scenes.scene2d.ui.ProgressBar;
import com.badlogic.gdx.scenes.scene2d.utils.Drawable;
import com.badlogic.gdx.scenes.scene2d.utils.TextureRegionDrawable;
import jdk.internal.foreign.Utils;

public class HpBar extends ProgressBar {

    private Player player;
    public HpBar(Player player) {
        super(0f, 1f, 0.01f, false, new ProgressBarStyle());
        this.player = player;

        int height = 30;
        this.setPosition(50, Gdx.graphics.getHeight() - 50);
        getStyle().background = getColoredDrawable(player.getHpMax()*2, height, Color.RED);
        getStyle().knob = getColoredDrawable(0, height, Color.GREEN);
        getStyle().knobBefore = getColoredDrawable(player.getHpMax()*2, height, Color.GREEN);

        setWidth(player.getHpMax()*2);
        setHeight(height);

        setAnimateDuration(0.0f);
        setValue(1f);

        setAnimateDuration(0.25f);
    }

    public void update(){this.setValue(player.getHp() / player.getHpMax());}



    public Drawable getColoredDrawable(int width, int height, Color color) {
        Pixmap pixmap = new Pixmap(width, height, Pixmap.Format.RGBA8888);
        pixmap.setColor(color);
        pixmap.fill();

        TextureRegionDrawable drawable = new TextureRegionDrawable(new TextureRegion(new Texture(pixmap)));

        pixmap.dispose();

        return drawable;
    }


}
